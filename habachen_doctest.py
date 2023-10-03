import doctest
import sys

import habachen


globs = dict(habachen=habachen)

flags = (doctest.IGNORE_EXCEPTION_DETAIL
       | doctest.REPORT_NDIFF
       | doctest.FAIL_FAST)

targets = [r"README.md", r"docs/_sources/habachen_module.rst.txt"]

for target in targets:
    failures, _ = doctest.testfile(
        target, globs=globs, optionflags=flags)

    if failures: sys.exit(1)
