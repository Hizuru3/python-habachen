# Habachen
Habachen provides a fast implementation of Japanese text converters that support half-width/full-width characters but also hiragana/katakana.

## License

[MIT License](https://github.com/Hizuru3/python-habachen/blob/main/LICENSE)

## Features

- Faster than existing Japanese text converter modules for Python; even faster than mojimoji, which is written in Cython/C++.

- No dependency on other third-party libraries at runtime.

- Supports Python 3.8, 3.9, 3.10, 3.11, 3.12 and 3.13.

## Installation

```sh
pip install habachen
```

## Usage

```python
# First, import it!
>>> import habachen

# Hankaku (half-width) to zenkaku (full-width)
>>> habachen.han_to_zen('abc!?012ﾊﾝｶｸﾓｼﾞ')
'ａｂｃ！？０１２ハンカクモジ'

# Convert only hankaku katakana
>>> text = 'abc!?012ﾊﾝｶｸﾓｼﾞ'
>>> habachen.han_to_zen(text, ascii=False, digit=False, kana=True)
'abc!?012ハンカクモジ'

# Zenkaku to hankaku
>>> habachen.zen_to_han('ａｂｃ！？０１２ゼンカクモジ')
'abc!?012ｾﾞﾝｶｸﾓｼﾞ'

# Convert zenkaku characters except katakana
>>> text = 'ａｂｃ！？０１２ゼンカクモジ'
>>> habachen.zen_to_han(text, kana=False)
'abc!?012ゼンカクモジ'

# Hiragana to katakana
>>> habachen.to_katakana('もじれつの変換')
'モジレツノ変換'

# Ignore specific hiragana
>>> habachen.to_katakana('あいうえおかきくけこ', ignore='あか')
'あイウエオかキクケコ'

# Hiragana to hankaku katakana
>>> habachen.to_katakana('もじれつの変換', hankaku=True)
'ﾓｼﾞﾚﾂﾉ変換'

# Zenkaku katakana to hiragana
>>> habachen.to_hiragana('モジレツノ変換')
'もじれつの変換'

# Ignore specific katakana
>>> habachen.to_hiragana('テストデス', ignore='ス')
'てスとでス'

```

## Benchmarks

### Target text
*Dogura magura* by Kyūsaku Yumeno  
http://www.aozora.gr.jp/cards/000096/files/2093_ruby_28087.zip
(public domain)

### Python version
Python 3.10.12 (main, Jun 11 2023, 05:26:28) [GCC 11.4.0] on linux

### Library versions
habachen-0.3.0  
mojimoji-0.0.12  
jaconv-0.3.4  

### Results: text with 140 characters

| | habachen | mojimoji | jaconv |
|---|---|---|---|
| zenkaku to hankaku | **1.319 µs** | 11.92 µs | 11.22 µs |
| hankaku to zenkaku | **1.147 µs**  | 10.15 µs | 26.49 µs |
| hiragana to katakana | **0.3674 µs** |  | 11.22 µs |
| katakana to hiragana | **0.3542 µs**  |  | 10.97 µs | 
  
### Results: text with 468996 characters

| | habachen | mojimoji | jaconv |
|---|---|---|---|
| zenkaku to hanaku | **2.607 ms** | 55.07 ms | 40.36 ms |
| hankaku to zenkaku | **1.832 ms**  | 33.89 ms | 57.16 ms |
| hiragana to katakana | **0.711 ms** |  | 38.72 ms |
| katakana to hiragana | **0.755 ms**  |  | 40.36 ms |
  
Script for benchmarking is [here](https://github.com/Hizuru3/python-habachen/blob/main/benchmarks/dogura.ipynb)

## Links (Japanese)

Documentation:
https://hizuru3.github.io/python-habachen/

Qiita article:
https://qiita.com/Hizuru/items/6b91d00fbe13ebb43867
