import os
import sys

# Let the tests use the Back End's flat imports (e.g. "from accounts_file import AccountsFile").
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
