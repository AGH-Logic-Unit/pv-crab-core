import os
import sys
from cocotb_tools.runner import get_runner

# Add verification directories to python path programmatically
sys.path.append(os.path.abspath("verif/addsub"))

def run_cocotb_test(module_name: str, test_module: str, seed: int = 42):
    """
    Helper function to run a cocotb test with clean industry-style directory sorting.
    Build files go to regression/build/<module_name>
    Test results and coverage reports go to regression/results/<module_name>/<test_module>
    """
    base_dir = os.path.abspath("regression")
    build_dir = os.path.join(base_dir, "build", module_name)
    results_dir = os.path.join(base_dir, "results", module_name, test_module)

    os.makedirs(build_dir, exist_ok=True)
    os.makedirs(results_dir, exist_ok=True)

    design_sources = [os.path.abspath(f"rtl/{module_name}/{module_name}.sv")]

    runner = get_runner("verilator")

    # 1. Compile the RTL design
    runner.build(
        sources=design_sources,
        hdl_toplevel=module_name,
        always=True,
        build_dir=build_dir,
        build_args=["--coverage"]
    )

    # 2. Run the test module
    runner.test(
        hdl_toplevel=module_name,
        test_module=test_module,
        seed=seed,
        build_dir=build_dir,
        results_xml=os.path.join(results_dir, "results.xml"),
        extra_env={"RESULTS_DIR": results_dir}
    )

def test_alu_runner():
    run_cocotb_test(module_name="addsub", test_module="tb_addsub")

def test_alu_feedback_runner():
    run_cocotb_test(module_name="addsub", test_module="tb_addsub_feedback")

def test_alu_z3_runner():
    run_cocotb_test(module_name="addsub", test_module="tb_addsub_z3")

if __name__ == "__main__":
    print("Running test_alu_runner...")
    test_alu_runner()
    print("Running test_alu_feedback_runner...")
    test_alu_feedback_runner()
    print("Running test_alu_z3_runner...")
    test_alu_z3_runner()
