from setuptools import setup, Extension


ext = Extension('habachen._habachen',
                sources = ['ext/habachen.c'],
                extra_compile_args=[])

setup (name = 'habachen',
       version = '0.6.0',
       description = 'Yet Another Fast Japanese String Converter',
       author = 'Hizuru',
       url = 'https://github.com/Hizuru3/python-habachen',
       license = 'MIT',
       license_files = ['LICENSE'],
       long_description = '''
Habachen provides a fast implementation of Japanese text converters
that support half-width/full-width characters but also hiragana/katakana.
''',
       packages = ['habachen'],
       package_data = {'habachen': ['py.typed', '__init__.pyi']},
       ext_modules = [ext],
       classifiers = ['Intended Audience :: Science/Research',
                      'Intended Audience :: Developers',
                      'Natural Language :: Japanese',
                      'Programming Language :: Python :: 3.8',
                      'Programming Language :: Python :: 3.9',
                      'Programming Language :: Python :: 3.10',
                      'Programming Language :: Python :: 3.11',
                      'Programming Language :: Python :: 3.12',
                      'Programming Language :: Python :: 3.13',
                      'Programming Language :: Python :: 3.14',
                      'Programming Language :: Python :: Implementation :: CPython',
                      'Topic :: Text Processing :: Linguistic'],
       python_requires = '>=3.8')





