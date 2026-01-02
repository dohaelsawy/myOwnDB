package pkg
import(
	"strings"
)

var cmdList = map[string]bool{"exit": true, "insert": true, "select": true}

func PrintPrompt() {
	print("db> ")
}

func (b *Buffer) CompareStr(cmd string) bool {
	return cmdList[cmd]
}

func (b *Buffer) TokenizeInput() []string{

	tokens := strings.Fields(b.Buffer)
	return  tokens
}