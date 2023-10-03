import sys

if sys.version_info >= (3, 9):
    from collections.abc import Iterable
else:
    from typing import Iterable


__version__: str

def hankaku_to_zenkaku(
        text: str, /, *,
        ascii: bool = True, digit: bool = True, kana: bool = True) -> str:
    """
    Return a copy of the given string with hankaku (half-width) characters
    converted to their corresponding zenkaku (full-width) counterparts.
    
    Parameters
    ----------
    text : str
        String to covert.
    ascii : bool, default=True
        Whether to convert ASCII characters (except digits).
    digit : bool, default=True
        Whether to convert ASCII digit characters.
    kana : bool, default=True
        Whether to convert hankaku katakana to zenkaku katakana.
    
    Returns
    ------
    str
        New string with converted characters.
    """

def to_zenkaku(
        text: str, /, *,
        ascii: bool = True, digit: bool = True, kana: bool = True) -> str:
    "Alias for hankaku_to_zenkaku()"

def han_to_zen(
        text: str, /, *,
        ascii: bool = True, digit: bool = True, kana: bool = True) -> str:
    "Alias for hankaku_to_zenkaku()"


def zenkaku_to_hankaku(
        text: str, /, *,
        ascii: bool = True, digit: bool = True, kana: bool = True) -> str:
    """
    Return a copy of the given string with zenkaku (full-width) characters
    converted to their corresponding hankaku (half-width) counterparts.
    
    Parameters
    ----------
    text : str
        String to covert.
    ascii : bool, default=True
        Whether to convert to ASCII characters (except digits).
    digit : bool, default=True
        Whether to convert to ASCII digit characters.
    kana : bool, default=True
        Whether to convert to hankaku katakana from zenkaku katakana.
    
    Returns
    ------
    str
        New string with converted characters.
    """

def to_hankaku(
        text: str, /, *,
        ascii: bool = True, digit: bool = True, kana: bool = True) -> str:
    "Alias for zenkaku_to_hankaku()"

def zen_to_han(
        text: str, /, *,
        ascii: bool = True, digit: bool = True, kana: bool = True) -> str:
    "Alias for zenkaku_to_hankaku()"


def hiragana_to_katakana(
        text: str, /,
        ignore: Iterable[str] = '', *, hankaku: bool = False) -> str:
    """
    Return a copy of the given string with hiragana characters converted
    to their corresponding katakana counterparts.
    
    Parameters
    ----------
    text : str
        String to covert.
    ignore : iterable of str, optional
        Hiragana characters to be ignored in conversion. All the
        characters must be convertible to their corresponding katakana.
    hankaku : bool, default=False
        Whether to convert to hankaku (half-width) katakana or zenkaku
        (full-width) katakana. The default is zenkaku.
    
    Returns
    ------
    str
        New string with converted characters.
    """

def to_katakana(
        text: str, /,
        ignore: Iterable[str] = '', *, hankaku: bool = False) -> str:
    "Alias for hiragana_to_katakana()"

def hira_to_kata(
        text: str, /,
        ignore: Iterable[str] = '', *, hankaku: bool = False) -> str:
    "Alias for hiragana_to_katakana()"


def katakana_to_hiragana(
        text: str, /, ignore: Iterable[str] = '') -> str:
    """
    Return a copy of the given string with zenkaku (full-width) katakana
    converted to their corresponding hiragana.
    
    Parameters
    ----------
    text : str
        String to covert.
    ignore : iterable of str, optional
        Katakana characters to be ignored in conversion. All the
        characters must be convertible to their corresponding hiragana.
    
    Returns
    ------
    str
        New string with converted characters.
    """

def to_hiragana(
        text: str, /, ignore: Iterable[str] = '') -> str:
    "Alias for katakana_to_hiragana()"

def kata_to_hira(
        text: str, /, ignore: Iterable[str] = '') -> str:
    "Alias for katakana_to_hiragana()"
