# scripts/conftest.py
# Pytest configuration and custom hooks

def pytest_configure(config):
    # Ignore the cocotb-test kwargs deprecation warning globally by message matching
    config.addinivalue_line(
        "filterwarnings",
        "ignore:.*Using kwargs is deprecated.*"
    )

def pytest_addoption(parser):
    parser.addoption(
        "--testlist",
        action="append",
        default=[],
        help="Path to a specific YAML testlist (can be specified multiple times)"
    )
    parser.addoption(
        "--filelist",
        action="store",
        default=None,
        help="Path to a SystemVerilog filelist (.f or .filelist)"
    )
    parser.addoption(
        "--seed",
        action="store",
        default=None,
        help="Specify randomization seed"
    )
    parser.addoption(
        "--waves",
        action="store_true",
        default=False,
        help="Enable waveform dumping during simulation"
    )
    parser.addoption(
        "--coverage",
        action="store_true",
        default=False,
        help="Force enable coverage collection during simulation"
    )
