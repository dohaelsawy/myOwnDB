package pkg

import (
	"bufio"
	"os"
)

type Buffer struct {
	buffer string
	bufferLength int64 
	inputSize int64 
}


func NewInputBuffer() (*Buffer){
	buffer := new(Buffer)
	buffer.buffer = ""
	buffer.bufferLength = 0
	buffer.inputSize = 0
	return buffer
}


func (b *Buffer) ReadInput() bool {
	scanner := bufio.NewScanner(os.Stdin)

	ok := scanner.Scan()
	if !ok {
		return false
	}

	b.buffer = scanner.Text()
	b.inputSize = int64(len(b.buffer)-1)
	return true
}
