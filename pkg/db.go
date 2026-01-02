package pkg

import (
	"bufio"
	"fmt"
	"os"
)

type Buffer struct {
	Buffer string
	BufferLength int64 
	InputSize int64 
}


func NewInputBuffer() (*Buffer){
	buffer := new(Buffer)
	buffer.Buffer = ""
	buffer.BufferLength = 0
	buffer.InputSize = 0
	return buffer
}


func (b *Buffer) ReadInput() bool {
	scanner := bufio.NewScanner(os.Stdin)

	ok := scanner.Scan()
	if !ok {
		return false
	}

	b.Buffer = scanner.Text()
	b.InputSize = int64(len(b.Buffer)-1)
	return true
}

func (b *Buffer)ExecuteCmd() {

	tokens := b.TokenizeInput()
	fmt.Println(tokens)

	if !b.CompareStr(tokens[0]){
		fmt.Println("this commands doesn't exist yet :{")
		return
	}

	switch tokens[0] {
	case "exit":
		os.Exit(0)
	case "insert":
		fmt.Println("This is where we would do an insert.")
	case "select":
		fmt.Println("This is where we would do a select.")
	}
}
