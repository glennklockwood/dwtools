#!/usr/bin/env python
"""
Demonstrate basic interaction with the DataWarp REST API.

On NERSC systems, do `module load dws` on an internal node to load the
prerequisite CLI commands (dwgateway).
"""

import json
import argparse
import subprocess
import requests
import requests.packages.urllib3 as urllib3

# curl -H "Authorization: MUNGE cred=$(munge -n)" $(dwgateway)/dw/v1/${endpoint}/ > ${endpoint}.json

class DwGateway(object):
    """
    Simple handle to interact with the DataWarp REST API
    """
    def __init__(self, api_version=None, verify_ssl=False):
        """
        Set the necessary constants to use the DataWarp REST API
        """
        self.api_key = None
        self.gateway = None
        self.verify_ssl = verify_ssl
        self.set_gateway()
        self.set_api_key()
        self.base_uri = self.gateway.rstrip('/') + '/dw/'
        self.headers = {'Authorization' : ('MUNGE cred=%s' % self.api_key)}

        if api_version is None:
            avail_versions = self.get_api_versions()
            self.api_version = avail_versions[-1].rstrip('/') + '/'
        else:
            self.api_version = api_version.rstrip('/') + '/'

    def set_gateway(self):
        """
        Get a new DataWarp API endpoint
        """
        self.gateway = subprocess.check_output('dwgateway').strip()

    def set_api_key(self):
        """
        Get the MUNGE credential needed to talk to DataWarp
        """
        self.api_key = subprocess.check_output(['munge', '-n']).strip()

    def get_api_versions(self):
        """
        Ask DataWarp gateway about its supported API versions
        """
        result = requests.get(self.base_uri,
                              headers=self.headers,
                              verify=self.verify_ssl)
        dw_version_ids = []
        for dw_version in result.json()['versions']:
            if 'id' in dw_version:
                dw_version_ids.append(dw_version['id'])
        return dw_version_ids

    def get(self, path="", version=None):
        """
        Send a REST request to a DataWarp gateway
        """
        if version is None:
            version = self.api_version
        query_uri = self.base_uri + version + path
        return requests.get(query_uri,
                            headers=self.headers,
                            verify=self.verify_ssl)


def dw_rest_explore():
    """
    Issue a query the DataWarp REST API and return the results
    """
    parser = argparse.ArgumentParser(description='Explore the DataWarp REST API')
    parser.add_argument('path', type=str, nargs='?', default="", help="REST API path to query")
    parser.add_argument('--api-version', type=str, default=None, help="REST API version to use")
    parser.add_argument('--warnings', action='store_true', help="Print SSL certificate warnings")
    args = parser.parse_args()

    if not args.warnings:
        urllib3.disable_warnings()

    dwgateway = DwGateway()

    print "Our DataWarp gateway URI is %s" % dwgateway.base_uri

    print "Using API version %s (available versions: %s)" % (
        dwgateway.api_version,
        ', '.join(dwgateway.get_api_versions()))

    results = dwgateway.get(args.path)

    print json.dumps(results.json(), indent=4, sort_keys=True)
    
if __name__ == '__main__':
    dw_rest_explore()
