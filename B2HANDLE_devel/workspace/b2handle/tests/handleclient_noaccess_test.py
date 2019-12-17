"""Testing methods that need no server access."""

import sys
if sys.version_info < (2, 7):
    import unittest2 as unittest
else:
    import unittest
import json
sys.path.append("../..")
import b2handle.handleclient as b2handle
from b2handle.handleexceptions import HandleSyntaxError

class EUDATHandleClientNoaccessTestCase(unittest.TestCase):


    def setUp(self):
        self.inst = b2handle.EUDATHandleClient()

    def tearDown(self):
        pass

    # Init

    def test_constructor_no_args(self):
        """Test constructor without args: No exception raised."""
        inst = b2handle.EUDATHandleClient()
        self.assertIsInstance(inst, b2handle.EUDATHandleClient,
            'Not a client instance!')

    def test_constructor_with_url(self):
        """Test constructor with one arg (well-formatted server URL): No exception raised."""
        inst = b2handle.EUDATHandleClient('http://foo.bar')
        self.assertIsInstance(inst, b2handle.EUDATHandleClient,
            'Not a client instance!')

    def test_constructor_with_url(self):
        """Test constructor with one arg (ill-formatted server URL): No exception raised."""
        inst = b2handle.EUDATHandleClient('foo')
        self.assertIsInstance(inst, b2handle.EUDATHandleClient,
            'Not a client instance!')

    def test_instantiate_for_read_access(self):
        """Testing if instantiating with default handle server works. """

        # Create client instance with username and password
        inst = b2handle.EUDATHandleClient.instantiate_for_read_access()
        self.assertIsInstance(inst, b2handle.EUDATHandleClient)

    def test_instantiate_for_read_an_search(self):
        """Testing if instantiating with default handle server works. """

        # Create client instance with username and password
        inst = b2handle.EUDATHandleClient.instantiate_for_read_and_search(
            None, 'johndoe', 'passywordy')
        self.assertIsInstance(inst, b2handle.EUDATHandleClient)

    def test_instantiate_with_username_and_password_noindex(self):

        # Try to ceate client instance with username and password

        with self.assertRaises(HandleSyntaxError):
            inst = b2handle.EUDATHandleClient.instantiate_with_username_and_password(
                'someurl', 'johndoe', 'passywordy')

    # PID generation

    def test_generate_PID_name_without_prefix(self):
        """Test PID generation without prefix."""
        uuid = self.inst.generate_PID_name()
        self.assertFalse('/' in uuid,
            'There is a slash in the generated PID, even though no prefix was specified.')

    def test_generate_PID_name_with_prefix(self):
        """Test PID generation with prefix."""
        prefix = 'aprefix'
        uuid = self.inst.generate_PID_name(prefix)
        self.assertTrue(prefix+'/' in uuid,
            'The specified prefix is not present in the generated PID.')

    # Handle syntax

    def test_check_handle_syntax_normal(self):
        """Test check handle syntax"""
        syntax_checked = self.inst.check_handle_syntax("foo/bar")
        self.assertTrue(syntax_checked)

    def test_check_handle_syntax_two_slashes(self):
        """Handle Syntax: Exception if too many slashes in handle."""
        with self.assertRaises(HandleSyntaxError):
            self.inst.check_handle_syntax("foo/bar/foo")

    def test_check_handle_syntax_no_slashes(self):
        """Handle Syntax: Exception if too many slashes in handle."""
        with self.assertRaises(HandleSyntaxError):
            self.inst.check_handle_syntax("foobar")

    def test_check_handle_syntax_no_prefix(self):
        """Handle Syntax: Exception if no prefix."""
        with self.assertRaises(HandleSyntaxError):
            self.inst.check_handle_syntax("/bar")

    def test_check_handle_syntax_no_suffix(self):
        """Handle Syntax: Exception if no suffix."""
        with self.assertRaises(HandleSyntaxError):
            self.inst.check_handle_syntax("foo/")

    def test_check_handle_syntax_with_index(self):
        """Test check handle syntax with index."""
        syntax_checked = self.inst.check_handle_syntax("300:foo/bar")
        self.assertTrue(syntax_checked,
            'The syntax of the handle is not index:prefix/suffix.')

    def test_check_handle_syntax_none(self):
        """Test check handle syntax where handle is None"""
        with self.assertRaises(HandleSyntaxError):
            syntax_checked = self.inst.check_handle_syntax(None)

    def test_check_handle_syntax_with_index_nan(self):
        """Handle Syntax: Exception if index not a number."""
        with self.assertRaises(HandleSyntaxError):
            self.inst.check_handle_syntax_with_index("nonumber:foo/bar")

    def test_check_handle_syntax_with_index_noindex(self):
        """Handle Syntax: Exception if index not existent."""
        with self.assertRaises(HandleSyntaxError):
            self.inst.check_handle_syntax_with_index("index/missing")

    def test_check_handle_syntax_with_index_twocolons(self):
        """Handle Syntax: Exception if two colons."""
        with self.assertRaises(HandleSyntaxError):
            self.inst.check_handle_syntax_with_index("too:many:colons")

    def test_check_handle_syntax_with_index_onlyindex(self):
        """Handle Syntax: Exception if no prefix and suffix."""
        with self.assertRaises(HandleSyntaxError):
            self.inst.check_handle_syntax_with_index("onlyindex:")

    def test_remove_index(self):
        handle_with_index = "300:foo/bar"
        syntax_checked = self.inst.check_handle_syntax(handle_with_index)
        self.assertTrue(syntax_checked,
            'Test precondition failed!')
        index, handle = self.inst.remove_index(handle_with_index)
        syntax_checked = self.inst.check_handle_syntax(handle)
        self.assertTrue(syntax_checked,
            'After removing the index, the syntax of the handle should '+\
            'be prefix/suffix.')

    def test_remove_index_noindex(self):
        handle_with_index = "foo/bar"
        syntax_checked = self.inst.check_handle_syntax(handle_with_index)
        self.assertTrue(syntax_checked,
            'Test precondition failed!')
        index, handle = self.inst.remove_index(handle_with_index)
        syntax_checked = self.inst.check_handle_syntax(handle)
        self.assertTrue(syntax_checked,
            'After removing the index, the syntax of the handle should '+\
            'be prefix/suffix.')

    def test_remove_index_toomany(self):
        handle_with_index = "100:100:foo/bar"
        with self.assertRaises(HandleSyntaxError):
            index, handle = self.inst.remove_index(handle_with_index)

    # retrieve handle record (failing before any server access)

    def test_retrieve_handle_record_json_handlesyntax_wrong(self):
        """Test exception if handle syntax is wrong (retrieve_handle_record_json)."""

        with self.assertRaises(HandleSyntaxError):
            json_record = self.inst.retrieve_handle_record_json('testhandle')

    def test_retrieve_handle_record_when_handle_is_None(self):
        """Test error when retrieving a handle record with a None input."""

        # Call method and check result:
        with self.assertRaises(HandleSyntaxError):
            self.inst.retrieve_handle_record(None)

    # make_authentication_string

    def test_create_authentication_string(self):
        auth = self.inst.create_authentication_string('100:user/name', 'password123')
        expected = 'MTAwJTNBdXNlci9uYW1lOnBhc3N3b3JkMTIz'
        self.assertEquals(expected, auth,
            'Authentication string is: '+auth+', but should be: '+expected)

    # make_handle_url

    def test_make_handle_url(self):

        url = self.inst.make_handle_URL('testhandle')
        self.assertIn('/api/handles/', url,
            'No REST API path specified in URL: '+url)
        self.assertIn('handle.net', url,
            'handle.net missing in URL: '+url)
        self.assertNotIn('index=', url,
            'Index specified in URL: '+url)
        #self.assertIn('overwrite=false', url,
        #    'overwrite=false is missing: '+url)

    def test_make_handle_url_with_indices(self):

        url = self.inst.make_handle_URL('testhandle', [2,3,5])
        self.assertIn('/api/handles/', url,
            'No REST API path specified in URL: '+url)
        self.assertIn('index=2', url,
            'Index 2 specified in URL: '+url)
        self.assertIn('index=3', url,
            'Index 3 specified in URL: '+url)
        self.assertIn('index=5', url,
            'Index 5 specified in URL: '+url)
        #self.assertIn('overwrite=false', url,
        #    'overwrite=false is missing: '+url)

    def test_make_handle_url_overwrite_true(self):

        url = self.inst.make_handle_URL('testhandle', overwrite=True)
        self.assertIn('/api/handles/', url,
            'No REST API path specified in URL: '+url)
        self.assertIn('overwrite=true', url,
            'overwrite=true is missing: '+url)

    def test_make_handle_url_overwrite_false(self):

        url = self.inst.make_handle_URL('testhandle', overwrite=False)
        self.assertIn('/api/handles/', url,
            'No REST API path specified in URL: '+url)
        self.assertIn('overwrite=false', url,
            'overwrite=false is missing: '+url)

    def test_make_handle_url_otherurl(self):

        other = 'http://foo.foo'
        url = self.inst.make_handle_URL('testhandle', other_url=other)
        self.assertNotIn('/api/handles/', url,
            'REST API path should not be specified in URL: '+url)
        self.assertIn(other, url,
            'Other URL missing in URL: '+url)
        self.assertNotIn('handle.net', url,
            'handle.net should not be in URL: '+url)
        self.assertNotIn('index=', url,
            'Index specified in URL: '+url)
        #self.assertIn('overwrite=false', url,
        #    'overwrite=false is missing: '+url)
