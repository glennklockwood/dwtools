#!/usr/bin/env python
#
#  Basic interaction with the DataWarp API
#
#  To do:
#    1. REST error checking (wrap calls in proper Python)
#
#  On NERSC systems, do `module load dws` on an internal node to load the
#  prerequisite Python libraries
#

import requests
import subprocess

# curl -H "Authorization: MUNGE cred=$(munge -n)" $(dwgateway)/dw/v1/${endpoint}/ > ${endpoint}.json

def subprocess_check_output( cmd ):
    """
    because subprocess.check_output doesn't exist before Python 2.7
    """
    p = subprocess.Popen( cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
    out, _ = p.communicate()
    if p.returncode != 0:
        raise subprocess.CalledProcessError
    return out.strip()

def main():
    apikey = subprocess_check_output( [ 'munge', '-n' ] )
    dwgateway = subprocess_check_output( 'dwgateway' )
    headers = { 'Authorization' : ('MUNGE %s' % apikey) }
    uri = dwgateway.rstrip('/') + '/dw/'

    ### print topmost json
    print "Our DataWarp gateway URI is %s" % uri
    r = requests.get( uri, headers=headers )
    dw_versions = r.json()['versions']

    ### start looking for queryable versions
    dw_version_ids = []
    for dw_version in dw_versions:
        if 'id' in dw_version:
            dw_version_ids.append( dw_version['id'] )

    ### find toplevel API endpoints for each version
    dw_endpoints = []
    for dw_version_uri in [ uri + x for x in dw_version_ids ]:
        r = requests.get( dw_version_uri, headers=headers )
        versions_dict = r.json()
        print "found version uri %s" % dw_version_uri
        ### since we request only one version, versions.dict should contain
        ### exactly one item (the version we requested)
        for dw_version_id, dw_version_dict in versions_dict.iteritems():
            print "confirmed that %s issues a valid response" % dw_version_id
            for endpoint_dict in dw_version_dict:
                if 'id' in endpoint_dict:
                    print "  found endpoint: %s (%s)" % (
                        endpoint_dict['id'],
                        endpoint_dict['description'] if 'description' in endpoint_dict else '???'
                        )

if __name__ == '__main__':
    main()
