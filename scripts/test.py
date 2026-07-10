import os
import shutil
import random
import yaml
import pytest
from cocotb_test.simulator import run

ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def parse_filelist(path):
    sources = []
    includes = []
    defines = []
    if not os.path.exists(path):
        raise FileNotFoundError(f"Filelist file not found: {path}")

    with open(path, 'r', encoding='utf-8') as f:
        for line in f:
            # Strip comments and whitespace
            line = line.split('//')[0].split('#')[0].strip()
            if not line:
                continue

            if line.startswith('+incdir+'):
                inc = line[8:].strip()
                includes.append(inc)
            elif line.startswith('+define+'):
                df = line[8:].strip()
                defines.append(df)
            else:
                sources.append(line)

    return sources, includes, defines

def load_testlist(testlists, cli_filelist=None):
    if not testlists:
        raise ValueError("No testlist specified! Use --testlist <path>")

    test_cases = []
    test_ids = []
    for path in testlists:
        full_path = os.path.join(ROOT_DIR, path)

        if not os.path.exists(full_path):
            raise FileNotFoundError(f"Testlist file not found: {full_path}")

        # Load testlist
        try:
            with open(full_path, "r") as f:
                testlist = yaml.safe_load(f) or {}
        except yaml.YAMLError as e:
            raise ValueError(f"Failed to parse testlist YAML {full_path}: {e}")

        # Look only at "config" top-level key as requested
        config = testlist.get("config", {})

        # Determine testlist name: config name > folder/file name (localization string fallback)
        testlist_name = config.get("name")
        if not testlist_name:
            testlist_name = os.path.basename(os.path.dirname(path)) or os.path.splitext(os.path.basename(path))[0]

        for test in testlist.get("tests", []):
            if "name" not in test or "toplevel" not in test or "module" not in test:
                raise ValueError(f"Invalid test definition in {full_path}: each test must define 'name', 'toplevel', and 'module'")

            # Base settings from YAML
            verilog_sources = [os.path.join(ROOT_DIR, src) for src in test.get("verilog_sources", [])]
            includes = [os.path.join(ROOT_DIR, inc) for inc in test.get("includes", [])]
            defines = list(test.get("defines", []))

            # Filelist name resolution: CLI argument > testcase filelist > config filelist
            filelist_name = cli_filelist or test.get("filelist") or config.get("filelist")

            if filelist_name:
                # Resolve relative path first in testlist directory, then fallback to root
                testlist_dir = os.path.dirname(full_path)
                filelist_path = os.path.join(testlist_dir, filelist_name)
                if not os.path.exists(filelist_path):
                    filelist_path = os.path.join(ROOT_DIR, filelist_name)

                fl_sources, fl_includes, fl_defines = parse_filelist(filelist_path)

                verilog_sources.extend([os.path.join(ROOT_DIR, src) for src in fl_sources])
                includes.extend([os.path.join(ROOT_DIR, inc) for inc in fl_includes])
                defines.extend(fl_defines)
            else:
                if not verilog_sources:
                    raise ValueError(f"Test '{test['name']}' in {full_path} has no verilog_sources or filelist defined!")

            test_case = {
                "name": test["name"],
                "testlist_name": testlist_name,
                "toplevel": test["toplevel"],
                "module": test["module"],
                "verilog_sources": verilog_sources,
                "includes": includes,
                "defines": defines,
                "python_search": [os.path.dirname(full_path)],
                "simulator": config.get("simulator", "verilator"),
                "coverage": config.get("coverage", False),
                "clean": test.get("clean", config.get("default_clean", True)),
                "parameters": test.get("parameters", {}),
            }
            test_cases.append(test_case)
            test_ids.append(f"{testlist_name}::{test['name']}")

    return test_cases, test_ids

def pytest_generate_tests(metafunc):
    if "test_cfg" in metafunc.fixturenames:
        cli_testlists = metafunc.config.getoption("testlist")
        cli_filelist = metafunc.config.getoption("filelist")

        if not cli_testlists:
            raise ValueError("No testlist specified! Use --testlist <path>")

        test_cases, test_ids = load_testlist(cli_testlists, cli_filelist)
        metafunc.parametrize("test_cfg", test_cases, ids=test_ids)


def test_run(test_cfg, request):
    # Resolve seed: CLI > random seed
    seed = request.config.getoption("seed")
    if seed is None:
        seed = random.randint(0, 2**32 - 1)
    else:
        seed = int(seed)

    coverage_en = request.config.getoption("coverage")
    waves_en = request.config.getoption("waves")

    # Merge coverage
    coverage = test_cfg["coverage"] or coverage_en

    # Use a single shared build directory for all testcases to reuse Verilator build files
    sim_build_dir = os.path.join(ROOT_DIR, "output/sim_build")
    os.makedirs(sim_build_dir, exist_ok=True)

    # Clean up any leftover waves or results from previous runs BEFORE starting
    # to avoid copying old stale files
    for r, _, files in os.walk(sim_build_dir):
        for f in files:
            if "results.xml" in f or f.endswith((".vcd", ".fst")):
                try:
                    os.remove(os.path.join(r, f))
                except Exception:
                    pass

    print(f"\n[RUNNER] Starting test: {test_cfg['name']} with RANDOM_SEED={seed}")

    run(
        verilog_sources=test_cfg["verilog_sources"],
        includes=test_cfg["includes"],
        defines=test_cfg["defines"],
        python_search=test_cfg["python_search"],
        toplevel=test_cfg["toplevel"],
        module=test_cfg["module"],
        parameters=test_cfg["parameters"],
        simulator=test_cfg["simulator"],
        coverage=coverage,
        waves=waves_en,
        sim_build=sim_build_dir,
        clean=False,  # Force clean=False so cocotb-test doesn't delete compiled objects!
        extra_env={"RANDOM_SEED": str(seed)}
    )

    # Put outputs (waveforms, results, coverage) in regression/results/<testlist_name>/<test_name>/
    regression_dest = os.path.join(ROOT_DIR, "regression/results", test_cfg["testlist_name"], test_cfg["name"])
    if os.path.exists(regression_dest):
        shutil.rmtree(regression_dest)
    os.makedirs(regression_dest, exist_ok=True)

    # Copy files from the shared build directory and clean up useless simulator dumps
    for r, _, files in os.walk(sim_build_dir):
        for f in files:
            file_path = os.path.join(r, f)
            # Copy test results, waveforms, and coverage files
            # Note: Skip other compiler-generated .dat files (like Vtop__verFiles.dat)
            if f.endswith("results.xml") or f.endswith((".vcd", ".fst", "coverage.dat", "coverage.xml")):
                dst_name = "results.xml" if f.endswith("results.xml") else f
                dst_file = os.path.join(regression_dest, dst_name)

                os.makedirs(os.path.dirname(dst_file), exist_ok=True)
                shutil.copy2(file_path, dst_file)

                # Delete temporary waveforms and results from build directory to save space
                if f.endswith("results.xml") or f.endswith((".vcd", ".fst")):
                    try:
                        os.remove(file_path)
                    except Exception as e:
                        print(f"Warning: Could not remove temporary dump {file_path}: {e}")
