import unittest
import requests


class TestReflowApi(unittest.TestCase):

    def setUp(self):
        logger.info('Called set-up funciton')

    def test_ping_pong(self):
        response = requests.get('http://localhost:8080/api/v1/ReflowController/ping-pong')
        print(response.text)

if __name__ == '__main__':
    unittest.main()