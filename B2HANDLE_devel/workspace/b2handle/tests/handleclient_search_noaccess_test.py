"""Testing methods that need Handle server write access"""

import sys
if sys.version_info < (2, 7):
    import unittest2 as unittest
else:
    import unittest
import json
sys.path.append("../..")
from b2handle.handleclient import EUDATHandleClient
from b2handle.handleexceptions import ReverseLookupException

PATH_RES = 'resources'


class EUDATHandleClientSearchNoAccessTestCase(unittest.TestCase):

    def __init__(self, *args, **kwargs):
        unittest.TestCase.__init__(self, *args, **kwargs)

    def setUp(self):
        self.inst = EUDATHandleClient()
 

    def tearDown(self):
        pass

    def test_search_handle_for_forbiddenkeys(self):
        with self.assertRaisesRegexp(ReverseLookupException, 'Cannot search for key[.]*'):
            self.inst.search_handle(url='*dkrz*',
                                          checksum='*123*',
                                          anotherfield='xyz')
        
    def test_search_handle_for_fulltext(self):
        with self.assertRaisesRegexp(ReverseLookupException, 'Full-text search is not implemented yet[.]*'):
            self.inst.search_handle(url='*dkrz*',
                                          checksum='*123*',
                                          searchterms=['searchterm1', 'searchterm2'])

    def test_search_handle_noterms(self):
        with self.assertRaisesRegexp(ReverseLookupException, 'No search terms have been specified[.]*'):
            self.inst.search_handle()

    def test_create_revlookup_query_fulltext(self):
        with self.assertRaisesRegexp(ReverseLookupException, 'Full-text search is not implemented yet[.]*'):
            self.inst.create_revlookup_query('foo', 'bar')

    def test_create_revlookup_query_forbiddenkeys(self):
        with self.assertRaisesRegexp(ReverseLookupException, 'Cannot search for key[.]*'):
            self.inst.create_revlookup_query(foo='foo', bar='bar')

    def test_create_revlookup_query_noterms(self):
        with self.assertRaisesRegexp(ReverseLookupException, 'No search terms have been specified[.]*'):
            self.inst.create_revlookup_query()

    def test_create_revlookup_query_norestriction(self):
        inst = EUDATHandleClient(allowed_search_keys=[])
        query = inst.create_revlookup_query(baz='baz')
        self.assertEqual(query, '?baz=baz',
            'The query is: '+query)

    def test_create_revlookup_query_normal(self):
        query = self.inst.create_revlookup_query(URL='foo')
        self.assertEqual(query, '?URL=foo',
            'The query is: '+query)

    def test_instantiate_wrong_search_url(self):

        inst = EUDATHandleClient.instantiate_for_read_and_search(
            'someurl',
            'someuser',
            'somepassword',
            reverselookup_baseuri='http://something_random_foo_bar')

        self.assertIsInstance(inst, EUDATHandleClient)
