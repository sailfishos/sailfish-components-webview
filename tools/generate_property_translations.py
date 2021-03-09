#!/usr/bin/env python3
#
# Generates translation strings from gecko property files for lupdate
# to process

import os, os.path, sys, re, dataclasses, functools, argparse

DEFAULT_PREFIX = "sailfish_components_webview_popups-la"

class Generator:
    TRANSLATION = re.compile(r"(?P<name>[\w-]+)\s*=\s*(?P<text>.*)\s*")
    SPLITTER = re.compile(r"([A-Za-z0-9][a-z0-9]*)")
    PLACEHOLDER = re.compile(r"%(?:([0-9]+)\$)?S")

    def __init__(self, file, source):
        self.file = file
        self.source = source

    @staticmethod
    def _convert_key(name):
        """Convert from camelCase to snake_case"""
        return "_".join(Generator.SPLITTER.findall(name)).lower()

    @staticmethod
    def _convert_text(text):
        """Replace gecko's placeholders with Qt's and
        remove access key hints"""
        def replacer(match):
            return "%1" if match.group(1) is None else "%" + match.group(1)
        new = Generator.PLACEHOLDER.sub(replacer, text)
        while new != text:
            text = new
            new = Generator.PLACEHOLDER.sub(replacer, text)
        return new.replace("&", "")

    def process_line(self, line, lineno):
        """Processes one line of input and returns Translation objects"""
        match = self.TRANSLATION.match(line)
        if match is not None:
            return Translation(self.source, lineno, match.group('name'),
                               self._convert_key(match.group('name')),
                               self._convert_text(match.group('text')))

    def __iter__(self):
        try:
            for lineno, line in enumerate(self.file, 1):
                result = self.process_line(line, lineno)
                if result is not None:
                    yield result
        except UnicodeDecodeError:
            print(f"Invalid character in file: {self.file.name}",
                  file=sys.stderr)
            raise

@dataclasses.dataclass
class Translation:
    source: str
    lineno: int
    original_key: str
    key: str
    text: str

    prefix = None

    def __str__(self):
        return f"""
//: This is a transformed translation key, the original translation key
//: ({self.original_key}) is from {self.source} at line {self.lineno}
//% "{self.text}"
const auto {self.key} = qtTrId("{self.prefix}-{self.key}");"""

def generate(file, url):
    print("/* Generated dummy file, do not edit */")
    for translation in Generator(file, url):
        print(translation)
    print("/* End of translations */")

def main():
    parser = argparse.ArgumentParser(description="""
    URL should be pointing to the exact file and version in version control.
    Use permalink that includes git hash and file path.""")
    parser.add_argument("file", metavar="FILE", type=argparse.FileType(),
            help="Property file to read")
    parser.add_argument("--prefix", "-p", default=DEFAULT_PREFIX,
            help=f"Prefix to use in translation keys [default: {DEFAULT_PREFIX}]")
    parser.add_argument("--url", default="unknown source",
            help="Url to source file [default: unknown source]")
    args = parser.parse_args()
    Translation.prefix = args.prefix
    generate(args.file, args.url)

if __name__ == "__main__":
    main()
