#https://docs.pytest.org/en/latest/reference.html?highlight=addoption#_pytest.hookspec.pytest_addoption
#https://stackoverflow.com/questions/42778124/passing-command-line-arguments-in-python-by-pytest
def pytest_addoption(parser):
    ''' attaches optional cmd-line args to the pytest machinery '''
    parser.addoption("--server_executable", action="append", default=[], help="path to reflow controller server executable")
    parser.addoption("--server_config_dir", action="append", default=[], help="path to reflow controller server cwd with config")