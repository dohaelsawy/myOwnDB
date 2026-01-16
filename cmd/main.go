package main

import (
	"os"

	pkg "github.com/dohaelsawy/myOwnDB/pkg"
)




func main() {
	table := pkg.NewTable()
	buffer := pkg.NewInputBuffer()
	for {
		pkg.PrintPrompt()
		ok := buffer.ReadInput()
		if !ok {
			os.Exit(1)
		}
		buffer.ExecuteCmd(table)
	}
}