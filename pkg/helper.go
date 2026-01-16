package pkg

import (
	"strings"
)

type Cmd string

const (
	EXIT Cmd = "exit"
	INSERT Cmd = "insert"
	SELECT Cmd = "select"
)

func (c Cmd) String() string{
	switch c {
	case EXIT:
		return "exit"
	case INSERT:
		return "insert"
	case SELECT:
		return "select"
	}
	return ""
}


func (b *Buffer) ExistCmd(cmd Cmd) bool {
	if len(cmd.String()) == 0 {
		return false
	} 
	return true
}

func (b *Buffer) TokenizeInput() []string{
	tokens := strings.Fields(b.Buffer)
	return  tokens
}

func PrintPrompt() {
	print("db> ")
}

