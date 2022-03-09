import unittest
import urllib
from urllib import response
from wsgiref.util import request_uri
import requests
import json
import logging

class TestReflowApi(unittest.TestCase):
    logging.basicConfig(level=logging.INFO)

    @classmethod
    def setUpClass(cls):
        cls.base_url = 'http://localhost:8086/api/v1/ReflowController/'

    def test_ping_pong(self):
        response = requests.get(urllib.parse.urljoin(self.base_url,'ping-pong'))

        self.assertTrue(response.ok)

        response_json = json.loads(response.text)
        self.assertEquals(response_json["message"], 'Pong')

    def test_create_new_preset(self):
        preset_name = 'Mechanic XG-50'
        preset_id = self.create_preset_with_name(preset_name)
        self.assertTrue(preset_id)

    def test_add_temperature_stages_to_preset(self):
        #http://www.laser-trafaret.ru/solderpaste_profile.php
        preset_name = 'SMT9603005-38'
        preset_id = self.create_preset_with_name(preset_name)
        preset_stages = [{'temperature':150, 'duration':60},{'temperature':180, 'duration':80}]

        self.add_stages_to_preset(preset_id,preset_stages)

        current_preset_data = self.get_preset_data(preset_id)
        self.assertEquals(current_preset_data['preset-name'], preset_name)

        received_stages = current_preset_data['stages']
        self.assertEquals(len(preset_stages), len(received_stages))

    def test_post_commands_to_reflow_table(self):
        self.post_command_to_server('start')
        self.post_command_to_server('stop')

    def create_preset_with_name(self, preset_name):
        payload = {'preset-name':preset_name}
        create_preset_url = urllib.parse.urljoin(self.base_url, 'preset')

        response = requests.post(create_preset_url,data=json.dumps(payload))

        self.assertTrue(response.ok)
        return json.loads(response.text)["preset-id"]

    def add_stages_to_preset(self,preset_id, stages_container):
        stages_request_data = {'stages':stages_container}
        stages_payload = json.dumps(stages_request_data)
        request_url = self.join_url_with_preset_id(preset_id)

        response = requests.put(request_url, data=stages_payload)
        self.assertTrue(response.ok)

    def get_preset_data(self, preset_id):
        request_url = self.join_url_with_preset_id(preset_id)

        response = requests.get(request_url)
        self.assertTrue(response.ok)
        return json.loads(response.text)

    def join_url_with_preset_id(self,preset_id):
        request_url = urllib.parse.urljoin(self.base_url, 'preset/')
        request_url = urllib.parse.urljoin(request_url,str(preset_id))
        return request_url

    def post_command_to_server(self,command):
        request_url = urllib.parse.urljoin(self.base_url, 'command')
        command_payload = {'command': command}
        response = requests.post(request_url,data=json.dumps(command_payload))
        self.assertTrue(response.ok)

if __name__ == '__main__':
    unittest.main()