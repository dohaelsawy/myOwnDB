package main

import (
	"os"

	pkg "github.com/dohaelsawy/myOwnDB/pkg"
)

var CmdList = []string{"exit"}


func main() {
	buffer := pkg.NewInputBuffer()
	for {
		pkg.PrintPrompt()
		ok := buffer.ReadInput()
		if !ok {
			os.Exit(1)
		}
		if buffer.CompareStr(CmdList[0]){
			os.Exit(0)
		}
		println("this command isn't valid")
	}
}