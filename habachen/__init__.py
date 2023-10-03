from ._habachen import (
    hankaku_to_zenkaku, zenkaku_to_hankaku,
    hiragana_to_katakana, katakana_to_hiragana)

from ._version import __version__


__all__ = [
    'hankaku_to_zenkaku', 'to_zenkaku', 'han_to_zen',
    'zenkaku_to_hankaku', 'to_hankaku', 'zen_to_han',
    'hiragana_to_katakana', 'to_katakana', 'hira_to_kata',
    'katakana_to_hiragana', 'to_hiragana', 'kata_to_hira']

to_zenkaku = han_to_zen = hankaku_to_zenkaku
to_hankaku = zen_to_han = zenkaku_to_hankaku
to_katakana = hira_to_kata = hiragana_to_katakana
to_hiragana = kata_to_hira = katakana_to_hiragana
