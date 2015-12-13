[Python标准库]re——正则表达式
    作用：使用形式化模式搜索和修改文本。
    Python 版本：1.5 及以后版本
    正则表达式（regular expression）是用一种形式化语法描述的文本匹配模式。模式被解释为一组命令，以一个字符串作为输入，生成一个匹配的子集或原字符串的修改版本。“正则表达式”一词在讨论中通常会简写为“regex”或“regexp”。表达式可以包括字面量文本匹配、重复、模式组合、分支以及其他复杂的规则。对于很多解析问题，用正则表达式解决会比创建特殊用途的词法分析器和语法分析器更为容易。
    正则表达式通常在涉及大量文本处理的应用中使用。例如，在开发人员使用的文本编辑程序中（包括 vi、emacs 和其他现代 IDE）常用正则表达式作为搜索模式。另外，正则表达式还是 UNIX 命令行工具的一个不可缺少的部分，如 sed、grep 和 awk。很多编程语言都在语言语法中包括对正则表达式的支持，如 Perl、Ruby、Awk 和 Tcl。另外一些语言（如 C、C++ 和 Python）则通过扩展库来支持正则表达式。
    有很多开源的正则表达式实现，这些实现都有一种共同的核心语法，不过对其高级特性有不同的扩展或修改。Python 的 re 模块中使用的语法以 Perl 所用正则表达式语法为基础，并提供了一些特定于 Python 的改进。
查找文本中的模式
    re 最常见的用法就是搜索文本中的模式。search() 函数取模式和要扫描的文本作为输入，如果找到这个模式则返回一个 Match 对象。如果未找到模式，search() 将返回 None。
    每个 Match 对象包含有关匹配性质的信息，包括原输入字符串、使用的正则表达式，以及模式在原字符串中出现的位置。

import re

pattern = 'this'
text = 'Does this text match the pattern?'

match = re.search(pattern, text)

s = match.start()
e = match.end()

print 'Found "%s"\nin "%s"\nfrom %d to %d ("%s")' % \
    (match.re.pattern, match.string, s, e, text[s:e])

编译表达式
    re 包含一些模块级函数，用于处理作为文本字符串的正则表达式，不过对于程序频繁使用的表达式，编译这些表达式会更为高效。compile() 函数会把一个表达式字符串转换为一个 RegexObject。

import re

# Precompile the patterns
regexes = [ re.compile(p) for p in ['this', 'that']]

text = 'Does this text match the pattern?'

print 'Text: %r\n' % text

for regex in regexes:
	print 'Seeking "%s" ->' % regex.pattern,

	if regex.search(text):
		print 'match!'
	else:
		print 'no match'

    模块级函数会维护已编译表达式的一个缓存。不过，这个缓存的大小是有限的，直接使用已编译表达式可以避免缓存查找的开销。使用已编译表达式的另一个好处是，通过在加载模块时预编译所有的表达式，可以把编译工作转到应用开始时，而不是当程序响应一个用户动作时才进行编译。
多重匹配
    到目前为止，示例模式都使用 search() 来查找字面量文本字符串的单个实例。findall() 函数会返回输入中与模式匹配而不重叠的所有子串。

import re

text = 'abbaaabbbbaaaaa'

pattern = 'ab'

for match in re.findall(pattern, text):
	print 'Found "%s"' % match

    这个输入字符串中有两个 ab 实例
    finditer() 会返回一个迭代器，它将生成 Match 实例，而不像 findall() 返回字符串。

import re

text = 'abbaaabbbbaaaaa'

pattern = 'ab'

for match in re.finditer(pattern, text):
	s = match.start()
	e = match.end()
	print 'Found "%s" at %d:%d' % (text[s:e], s, e)

    这个例子找到了 ab 的两次出现，Match 实例显示出它们在原输入字符串中的位置。
模式语法
    正则表达式支持更强大的模式，而不只是简单的字面量文本字符串。模式可以重复，可以锁定到输入中不同的逻辑位置，还可以采用紧凑形式表示而不需要在模式中提供每一个字面量字符。使用所有这些特性时，需要结合字面量文本值和元字符（metacharacter），元字符是 re 实现的正则表达式模式语法的一部分。

# re_test_patterns.py

import re

def test_patterns(text, patterns=[]):
	"""Given source text and a list of patterns, look for 
	matches for each pattern within the taxt and print
	them to stdout.
	"""

	# Look for each pattern in the text and print the results
	for pattern, desc in patterns:
		print 'Pattern %r (%s)\n' % (pattern, desc)
		print '  %r' % text
		for match in re.finditer(pattern, text):
			s = match.start()
			e = match.end()
			substr = text[s:e]
			n_backslashes = text[:s].count('\\')
			prefix = '.' * (s + n_backslashes)
			print '  %s%r' % (prefix, substr)
		print
	return

if __name__ == '__main__':
	test_patterns('abbaaabbbbaaaaa', [('ab', "'a' followed by 'b'"),])

    上面的例子使用 test_patterns() 来研究模式变化如何影响以何种方式匹配相同的输入文本。输出显示了输入文本以及输入中与模式匹配的各个部分的子串区间。
    1. 重复
    模式中有 5 中表达重复的方式。如果模式后边跟有元字符 “*”，这个模式会重复 0 次或多次。如果将 “*” 替换为 “+”，那么这个模式必须至少出现 1 次。使用 “?” 意味着模式要出现 0 或 1 次。如果希望出现特定的次数，需要在模式后边使用 {m}，这里 m 是模式需要重复的次数。最后，如果允许重复次数可变但是有一个限定范围，可以使用 {m,n}，这里 m 是最小重复次数，n 是最大重复次数。如果省略 n（即 {m,}），表示这个值至少要出现 m 次， 但无上限。

from re_test_patterns import test_patterns

test_patterns(
	'abbaabbba',
	[ ('ab*', 'a followed by zero or more b'),
	  ('ab+', 'a followed by one or more b'),
	  ('ab?', 'a followed by zero or one b'),
	  ('ab{3}', 'a followed by three b'),
	  ( 'ab{2,3}', 'a followed by two to three b'),
	])

    ab* 和 ab? 的匹配模式要多于 ab+ 的匹配。
    正常情况下，处理重复指令时，re 匹配模式时会利用（consume）尽可能多的输入。这种所谓“贪心”的行为可能导致单个匹配减少，或者匹配中包含了多于原先预计的输入文本。在重复指令后面加上 “?” 可以关闭这种贪心行为。

from re_test_patterns import test_patterns

test_patterns(
	'abbaabbba',
	[ ('ab*?', 'a followed by zero or more b'),
	  ('ab+?', 'a followed by one or more b'),
	  ('ab??', 'a followed by zero or one b'),
	  ('ab{3}?', 'a followed by three b'),
	  ( 'ab{2,3}?', 'a followed by two to three b'),
	])

    2. 字符集
    字符集（character set）是一组字符，包含可以与模式中相应位置匹配的所有字符。例如，[ab] 可以匹配 a 或 b。

from re_test_patterns import test_patterns

test_patterns(
	'abbaabbba',
	[ ('[ab]', 'either a or b'),
	  ('a[ab]+', 'a followed by one or more a or b'),
	  ('a[ab]+?', 'a followed by one or more a or b, not greedy'),
	])

    表达式 (a[ab]+) 的贪心形式会利用整个字符串，因为第一个字母是 a，而且后面的每一个字符要么是 a 要么是 b。
    字符集还可以用来排除某些特定字符。尖字符（^）表示要查找未在随后的字符集中出现的字符。

from re_test_patterns import test_patterns

test_patterns(
	'This is some text -- with punctuation.',
	[ ('[^-. ]+', 'sequences without -, ., or space'),
	])

    这个模式将找到不包含字符 “-”、“.” 或空格的所有子串。
    随着字符集变大，键入每一个应当（或不应当）匹配的字符会变得很枯燥。可以使用一种更紧凑的格式，利用字符区间（character range）来定义一个字符集，其中包括一个起点和一个终点之间所有连续的字符。

from re_test_patterns import test_patterns

test_patterns(
	'This is some text -- with punctuation.',
	[ ('[a-z]+', 'sequences of lowercase letters'),
	  ('[A-Z]+', 'sequences of uppercase letters'),
	  ('[a-zA-Z]+', 'sequences of lowercase or uppercase letters'),
	  ('[A-Z][a-z]+', 'one uppercase followed by lowercase'),
	])

    这里的区间 a-z 包括所有小写 ASCII 字母，区间 A-Z 包括全部大写 ASCII 字母。这些区间还可以合并为一个字符集。
    作为字符集的一种特殊情况，元字符 “.”（点号）指模式应当匹配该位置的任何单字符。

from re_test_patterns import test_patterns

test_patterns(
	'abbaabbba',
	[ ('a.', 'a followed by any one character'),
	  ('b.', 'b followed by any one character'),
	  ('a.*b', 'a followed by anything, ending in b'),
	  ('a.*?b', 'a followed by anything, ending in b'),
	])

    结合点号与重复可以得到非常长的匹配结果，除非使用非贪心形式。
    3. 转义码
    还有一种更为紧凑的表示，可以对一些预定义的字符集使用转义码。re 可以识别的转义码如下所示。
    \d：一个数字
    \D：一个非数字
    \s：空白符（制表符、空格、换行符等）
    \S：非空白字符
    \w：字母数字
    \W：非字母数字
    注意：转义字符通过在该字符前面加一个反斜线（\）前缀来表示。遗憾的是，正常的 Python 字符串中反斜线自身也必须转义，这会导致表达式很难阅读。通过使用“原始”（raw）字符串（在字面值前面加一个前缀 r 来创建），可以消除这个问题，并维持可读性。

from re_test_patterns import test_patterns

test_patterns(
	'A prime #1 example!',
	[ (r'\d+', 'sequence of digits'),
	  (r'\D+', 'sequence of nondigits'),
	  (r'\s+', 'sequence of whitespace'),
	  (r'\S+', 'sequence of nonwhitespace'),
	  (r'\w+', 'alphanumeric characters'),
	  (r'\W+', 'nonalphanumeric'),
	])

    以上示例表达式结合了转义码和重复，来查找输入字符串中的类似字符序列。
    要匹配属于正则表达式语法的字符，需要对搜索模式中的字符进行转义。

from re_test_patterns import test_patterns

test_patterns(
	r'\d+ \D+ \s+',
	[ (r'\\.\+', 'escape code'),
	])

    这个例子中的模式对反斜线和加号字符进行了转义，因为作为元字符，这两个字符在正则表达式都有特殊的含义。
    4. 锚定
    除了描述要匹配的模式的内容外，还可以使用锚定（anchoring）指令指定输入文本中的模式应当出现的相对位置。下面列出了合法的锚定码。
    ^：字符串或行的开始
    $：字符串或行的结束
    \A：字符串的开始
    \Z：字符串的结束
    \b：一个单词开头或末尾的空串
    \B：不在一个单词开头或末尾的空串

from re_test_patterns import test_patterns

test_patterns(
	'This is some text -- with punctuation.',
	[ (r'^\w+', 'word at start of string'),
	  (r'\A\w+', 'word at start of string'),
	  (r'\w+\S*$', 'word near end of string, skip punctuation'),
	  (r'\w+\S*\Z', 'word near end of string, skip punctuation'),
	  (r'\w*t\w*', 'word containing t'),
	  (r'\bt\w+', 't at start of word'),
	  (r'\w+t\b', 't at end of word'),
	  (r'\Bt\B', 't, not start or end of word'),
	])

    这个例子中，匹配字符串开头和末尾单词的模式是不同的，因为字符串末尾的单词后面有一个结束句子的标点符号。模式 \w+$ 不能匹配，因为 “.” 并不是一个字母数字字符。
限制搜索
    如果提前已经知道只需搜索整个输入的一个子集，可以告诉 re 限制搜索范围，从而进一步约束正则表达式匹配。例如，如果模式必须出现在输入的最前面，那么使用 match() 而不是 search() 会锚定搜索，而不必在搜索模式中显式地包含一个锚。

import re

text = 'This is some text -- with punctuation.'
pattern = 'is'
print 'Text :', text
print 'Pattern :', pattern

m = re.match(pattern, text)
print 'Match :', m
s = re.search(pattern, text)
print 'Search :', s

    由于字面量文本 is 未出现在输入文本最前面，因此使用 match() 无法找到它。不过，这个序列在文本中另外出现了两次，所以 search() 可以找到它。
    已编译正则表达式的 search() 方法还接受可选的 start 和 end 位置参数，将搜索限制在输入的一个子串中。

import re

text = 'This is some text -- with punctuation.'
pattern = re.compile(r'\b\w*is\w*\b')

print 'Text :', text
print

pos = 0
while True:
	match = pattern.search(text, pos)
	if not match:
		break
	s = match.start()
	e = match.end()
	print '  %2d : %2d = "%s"' % (s, e-1, text[s:e])
	# Move forward in text for the next search
	pos = e

    这个例子实现了 iterall() 的一种不太高效的形式，每次找到一个匹配时，该匹配的结束位置将用于下一次搜索。
用组解析匹配
    搜索模式匹配是正则表达式所提供强大功能的基础。为模式增加组（group）可以隔离匹配文本的各个部分，进一步扩展这些功能来创建一个解析工具。通过将模式包围在小括号中（即 “(” 和 “)”）来分组。

from re_test_patterns import test_patterns

test_patterns(
	'abbaaabbbbaaaaa',
	[ ('a(ab)', 'a followed by literal ab'),
	  ('a(a*b*)', 'a followed by 0-n a and 0-n b'),
	  ('a(ab)*', 'a followed by 0-n ab'),
	  ('a(ab)+', 'a followed by 1-n ab'),
	  ])

    任何完整的正则表达式都可以转换为组，并嵌套在一个更大的表达式中。所有重复修饰符可以应用到整个组作为一个整体，这就要求重复整个组模式。
    要访问一个模式中单个组匹配的子串，可以使用 Match 对象的 groups() 方法。

import re

text = 'This is some text -- with punctuation.'

print text
print

patterns = [
    (r'^(\w+)', 'word at start of string'),
    (r'(\w+)\S*$', 'word at end, with optional punctuation'),
    (r'(\bt\w+)\W+(\w+)', 'word starting with t, another word'),
    (r'(\w+t)\b', 'word ending with t'),
    ]

for pattern, desc in patterns:
	regex = re.compile(pattern)
	match = regex.search(text)
	print 'Pattern %r (%s)\n' % (pattern, desc)
	print '  ', match.groups()
	print

    Match.groups() 会按表达式中与字符串匹配的组的顺序返回一个字符串序列。
    使用 group() 可以得到某个组的匹配，如果使用分组来查找字符串的各部分，不过结果中并不需要某些与组匹配的部分，此时 group() 会很有用。

import re

text = 'This is some text -- with punctuation.'

print 'Input text :', text

# word starting with 't' then another word
regex = re.compile(r'(\bt\w+)\W+(\w+)')
print 'Pattern                :', regex.pattern

match = regex.search(text)
print 'Entire match           :', match.group(0)
print 'Word starting with "t" :', match.group(1)
print 'Word after "t" word    :', match.group(2)

    第 0 组表示与整个表达式匹配的字符串，子组按其左小括号在表达式中出现的顺序从 1 开始标号。
    Python 对基本分组语法做了扩展，增加了命名组（named group）。通过使用名字来指示组，这样以后就可以更容易地修改模式，而不必同时修改使用了匹配结果的代码。要设置一个组的名字，可以使用以下语法：(?P<name>pattern)。

import re

text = 'This is some text -- with punctuation.'

print text
print

for pattern in [ r'(?P<first_word>\w+)',
                 r'(?P<last_word>\w+)\S*$',
                 r'(?P<t_word>\bt\w+)\W+(?P<other_word>\w+)',
                 r'(?P<ends_with_t>\w+t)\b',
                 ]:
    regex = re.compile(pattern)
    match = regex.search(text)
    print 'Matching "%s"' % pattern
    print '  ', match.groups()
    print '  ', match.groupdict()
    print

    使用 groupdict() 可以获取一个字典，它将组名映射到匹配的子串，groups() 返回的有序序列还包含命名模式。
    以下是更新后的 test_patterns()，它会显示与一个模式匹配的编号组和命名组，使后面的例子更容易理解。

# re_test_patterns_groups.py

import re

def test_patterns(text, patterns=[]):
	"""Given source text and a list of patterns, look for 
	matches for each pattern within the taxt and print
	them to stdout.
	"""

	# Look for each pattern in the text and print the results
	for pattern, desc in patterns:
		print 'Pattern %r (%s)\n' % (pattern, desc)
		print '  %r' % text
		for match in re.finditer(pattern, text):
			s = match.start()
			e = match.end()
			prefix = ' ' * (s)
			print '  %s%r%s ' % (prefix, text[s:e], ' '*(len(text)-e)),
			print match.groups()
			if match.groupdict():
				print '%s%s' % (' ' * (len(text)-s), match.groupdict())
		print
	return

    因为组本身也是一个完整的正则表达式，所以组可以嵌套在其他组中，够成更复杂的表达式。

from re_test_patterns_groups import test_patterns

test_patterns(
	'abbaabbba',
	[ (r'a((a*)(b*))', 'a followed by 0-n a and 0-n b'),
	 ])

    在这个例子中，组 (a*) 会匹配一个空串，所以 groups() 的返回值包括这个空串作为匹配值。
    组对于指定候选模式也很有用。可以使用管道符号 (|) 指示应该匹配某一个或另一个模式。不过，要仔细考虑管道符号的放置。下面这个例子中的第一个表达式会匹配一个 a 序列后面跟有一个完全由某一个字母 （a 或 b）构成的序列。第二个模式会匹配一个 a 后面跟有一个可能包含 a 或 b 的序列。这两个模式很相似，不过得到的匹配结果完全不同。

from re_test_patterns_groups import test_patterns

test_patterns(
	'abbaabbba',
	[ (r'a((a+)|(b+))', 'a then seq. of a or seq. of b'),
	  (r'a((a|b)+)', 'a then seq. of [ab]'),
	 ])

    如果候选组不匹配，但是整个模式确实匹配，groups() 的返回值会在序列中本应出现候选组的位置上包含一个 None 值。
    如果匹配子模式的字符串并不是从整个文本抽取的一部分，此时定义一个包含子模式的组也很有用。这些组称为“非捕获组”（noncapturing）。非捕获组可以用来描述重复模式或候选模式，而不在返回值中区分字符串的匹配部分。要创造一个非捕获组，可以使用语法 (?:pattern)。
    对于一个模式，尽管其捕获和非捕获形式会匹配相同的结果，但是会返回不同的组，下面来加以比较。

from re_test_patterns_groups import test_patterns

test_patterns(
	'abbaabbba',
	[ (r'a((a+)|(b+))', 'capturing form'),
	  (r'a((?:a+)|(?:b+))', 'noncapturing'),
	 ])
