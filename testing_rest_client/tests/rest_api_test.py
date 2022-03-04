import unittest
import requests
import json
import logging

class TestReflowApi(unittest.TestCase):
    logging.basicConfig(level=logging.INFO)

    def test_ping_pong(self):
        response = requests.get('http://localhost:8086/api/v1/ReflowController/ping-pong')
        logging.info(json.loads(response.text))

if __name__ == '__main__':
    unittest.main()