import pytest
import subprocess
import requests
import sys


def pytest_generate_tests(metafunc):
    path_to_server_executable = metafunc.config.getoption("server_executable")
    path_to_cwd = metafunc.config.getoption("server_config_dir")

    if path_to_server_executable:
        metafunc.cls.path_to_server_executable = path_to_server_executable[0]
    if path_to_cwd:
        metafunc.cls.path_to_cwd = path_to_cwd[0]

class TestReflowApi(object):

    path_to_server_executable = None
    path_to_cwd = None

    def test_ping_pong(self):
        print(self.path_to_server_executable)
        print("STARTED SERVER")
        subprocess.run(self.path_to_server_executable, cwd=self.path_to_cwd, shell=True, close_fds=True,stdin=None, stdout=None, stderr=None)
        response = requests.get("0:0:0:0:8080/api/v1/ReflowController/ping-pong")
        print(response.text)
