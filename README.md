[Read in English](https://github.com/Hizuru3/python-habachen/blob/main/README-en.md)

# Habachen
日本語の全角／半角とひらがな／カタカナの相互変換をサポートする高速で省メモリな文字列変換モジュールです。

## License

[MIT License](https://github.com/Hizuru3/python-habachen/blob/main/LICENSE)

## Installation

```sh
pip install habachen
```

## Usage

```python
# habachenモジュールをインポート
>>> import habachen

# 半角から全角
>>> habachen.han_to_zen('abc!?012ﾊﾝｶｸﾓｼﾞ')
'ａｂｃ！？０１２ハンカクモジ'

# 半角カタカナのみを変換
>>> text = 'abc!?012ﾊﾝｶｸﾓｼﾞ'
>>> habachen.han_to_zen(text, ascii=False, digit=False, kana=True)
'abc!?012ハンカクモジ'

# 全角から半角
>>> habachen.zen_to_han('ａｂｃ！？０１２ゼンカクモジ')
'abc!?012ｾﾞﾝｶｸﾓｼﾞ'

# 全角カタカナの変換を行わない
>>> text = 'ａｂｃ！？０１２ゼンカクモジ'
>>> habachen.zen_to_han(text, kana=False)
'abc!?012ゼンカクモジ'

# ひらがなをカタカナに変換
>>> habachen.to_katakana('もじれつの変換')
'モジレツノ変換'

# 指定したひらがなを無視
>>> habachen.to_katakana('あいうえおかきくけこ', ignore='あか')
'あイウエオかキクケコ'

# ひらがなを半角カタカナに変換
>>> habachen.to_katakana('もじれつの変換', hankaku=True)
'ﾓｼﾞﾚﾂﾉ変換'

# 全角カタカナをひらがなに変換
>>> habachen.to_hiragana('モジレツノ変換')
'もじれつの変換'

# 指定したカタカナを無視
>>> habachen.to_hiragana('テストデス', ignore='ス')
'てスとでス'

```

## Benchmarks

### 使用した文章
夢野久作『ドグラ・マグラ』  
http://www.aozora.gr.jp/cards/000096/files/2093_ruby_28087.zip

### Python のバージョン
Python 3.10.12 (main, Jun 11 2023, 05:26:28) [GCC 11.4.0] on linux

### ライブラリのバージョン
habachen-0.3.0  
mojimoji-0.0.12  
jaconv-0.3.4  

### 結果（短文 140文字）

| | habachen | mojimoji | jaconv |
|---|---|---|---|
| 全角→半角 | **1.319 µs** | 11.92 µs | 11.22 µs |
| 半角→全角 | **1.147 µs**  | 10.15 µs | 26.49 µs |
| ひらがな→カタカナ | **0.3674 µs** |  | 11.22 µs |
| カタカナ→ひらがな | **0.3542 µs**  |  | 10.97 µs | 
  
### 結果（全文 468996文字）

| | habachen | mojimoji | jaconv |
|---|---|---|---|
| 全角→半角 | **2.607 ms** | 55.07 ms | 40.36 ms |
| 半角→全角 | **1.832 ms**  | 33.89 ms | 57.16 ms |
| ひらがな→カタカナ | **0.711 ms** |  | 38.72 ms |
| カタカナ→ひらがな | **0.755 ms**  |  | 40.36 ms |
  
検証に用いたスクリプトは[こちら](https://github.com/Hizuru3/python-habachen/blob/main/benchmarks/dogura.ipynb)

## Links

ドキュメント：
https://hizuru3.github.io/python-habachen/

Qiita 記事：
https://qiita.com/Hizuru/items/6b91d00fbe13ebb43867
