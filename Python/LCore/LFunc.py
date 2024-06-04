
class LFunc:
    @staticmethod
    def escapeSpecialCharacters(text):
        specialCharacters = r'[\^$.|?*+(){}[]'
        escapedText = ''
        for ch in text:
            if ch in specialCharacters:
                escapedText += '\\'
            escapedText += ch
        return escapedText

    @staticmethod
    def replaceSpaceToHtml(text):
        return text.replace('\t', '&nbsp;&nbsp;&nbsp;&nbsp;').replace(' ', '&nbsp;')
