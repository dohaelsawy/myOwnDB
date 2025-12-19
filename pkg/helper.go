package pkg


func PrintPrompt() {
	print("db> ")
}

func (b *Buffer) CompareStr(cmd string) bool {
	if b.buffer == cmd {
		return true
	}
	return false
}