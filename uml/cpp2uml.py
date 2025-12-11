#!/usr/bin/python3
import sys, re

TYPE_GROUP = '(\w[\w *&<>:]*)'

REGEX_TABLE = [
	(re.compile(r'(?:explicit|noexcept|constexpr)\s+'),        ''),
	(re.compile(r'\s*//.*'),                                      ''),
	(re.compile(fr'^{TYPE_GROUP}\s+(\w+),[ \t]*(\w+)$'),       r'\1 \2\n\1 \3'), # multiple fields
	(re.compile(fr'{TYPE_GROUP}[ \t]+(\w+)'),                  r'\2: \1'),       # types
	(re.compile(r'^(\w+\([^)]+\)):.*'),                        r'\1'),           # remove constructor initializer
	(re.compile(r'^(\w+): (\w+)(\([^)]*\).*)$', re.MULTILINE), r'\1\3: \2'),     # function return value
	(re.compile(r': void$', re.MULTILINE),                     ''),              # remove void
	(re.compile(r'\{: ' + TYPE_GROUP + '$', re.MULTILINE),     ''),              # remove '{'
	(re.compile(r'[ \t]*=[^),]+'),                             ''),              # remove default values
]

def is_skip(line, level):
	return level != 0 or\
			line.startswith('~') or line.startswith('using') or line.startswith('#')

def process(filename):
	level = -1
	accessChar = '-'
	
	with open(filename, 'r') as file:
		for line in file.readlines():
			
			if level >= 0:
				end = line.find('}')
				if end != -1 and level == 0:
					print(line[:end+1].lstrip())
					level = -1
					continue

				line = line.lstrip()
				if line == '': continue
				
				if line.startswith('public:'):      accessChar = '+'
				elif line.startswith('protected:'): accessChar = '#'
				elif line.startswith('private:'):   accessChar = '-'
				elif not is_skip(line, level):
					header = line.replace(';', '')
					for regex, repl in REGEX_TABLE:
						header = re.sub(regex, repl, header)
					
					print(re.sub('^', '  ' + accessChar, header.rstrip(), 0, re.MULTILINE))
				
				level += line.count('{') - line.count('}')
				
				continue
				
			start = line.find('struct ')
			
			if start != -1 and '{' in line:
				print(line[start:], end='')
				level = 0


for filename in sys.argv[1:]:
	process(filename)
