package pkg

import (
	"bufio"
	"encoding/binary"
	"fmt"
	"os"
	"strconv"
	"unsafe"
)

const (
	TABLE_MAX_PAGES = 100
	PAGE_SIZE = 4096
	ID_OFFSET = 0
	
	ID_SIZE = unsafe.Sizeof(newRow().id)
	USERNAME_SIZE = unsafe.Sizeof(newRow().username)
	EMAIL_SIZE = unsafe.Sizeof(newRow().email)
	
	USERNAME_OFFSET = ID_OFFSET + ID_SIZE
	EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE
	ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE
	ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE
	TABLE_MAX_ROWS = ROWS_PER_PAGE * uintptr(TABLE_MAX_PAGES)
)

type Buffer struct {
	Buffer string
	BufferLength int64 
	InputSize int64 
}

type Table struct {
	Pages [][]byte
	RowsNum int
}

type row struct {
	id int 
	username string 
	email string
}

type statement struct {
	rowType string 
	insertRow row 
}


func NewInputBuffer() *Buffer{
	return &Buffer{
		Buffer: "",
		BufferLength: 0,
		InputSize: 0,
	}
}

func NewTable() *Table {
	return &Table{
		Pages: make([][]byte, TABLE_MAX_PAGES),
		RowsNum: 0,
	}
}

func newRow() *row {
	return &row{
		id: 0,
		username: "",
		email: "",
	}
}

func newStatement() *statement{
	return &statement{
		rowType: "",
		insertRow: *newRow(),
	}
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


func PrintRow(row *row) {
	fmt.Printf("(%d, %s,%s)\n", row.id, row.username, row.email)
}

func (r *statement) serializeRow(destination []byte) {
	buf := make([]byte, 8)
	// converts integer into 8 bytes to use copy function
	binary.LittleEndian.PutUint64(buf, uint64(r.insertRow.id))
	copy(destination[ID_OFFSET:], buf)
	copy(destination[USERNAME_OFFSET:], []byte(r.insertRow.username))
	copy(destination[EMAIL_OFFSET:], []byte(r.insertRow.email))
}

func (r *statement) deserializeRow(source []byte) {
	r.insertRow.id = int(binary.LittleEndian.Uint64(source[ID_OFFSET:]))
	r.insertRow.username = string(source[USERNAME_OFFSET : USERNAME_OFFSET+USERNAME_SIZE])
	r.insertRow.email = string(source[EMAIL_OFFSET : EMAIL_OFFSET+EMAIL_SIZE])
}

func (t *Table) rowSlot(rowNum int) []byte {
	pageNum := rowNum / int(ROWS_PER_PAGE)
	page := t.Pages[pageNum]
	if page == nil {
		t.Pages[pageNum] = make([]byte, PAGE_SIZE)
		page = t.Pages[pageNum]
	}
	rowOffset := rowNum % int(ROWS_PER_PAGE)
	byteOffset := rowOffset * int(ROW_SIZE)
	return page[byteOffset : byteOffset+int(ROW_SIZE)]
}

func (s *statement) prepareStatement(rowFields []string) {
	s.insertRow.id, _ = strconv.Atoi(rowFields[0])
	s.insertRow.username = rowFields[1]
	s.insertRow.email = rowFields[2]
}


func (t *Table) ExecuteInsert(statement *statement) string {
	if t.RowsNum >= TABLE_MAX_PAGES {
		return "table size is full"
	}
	statement.serializeRow(t.rowSlot(t.RowsNum))

	t.RowsNum += 1
	return  "success"
}


func (t *Table) ExecuteSelect(statement statement) string{
	for i := 0; i < t.RowsNum; i++ {
		statement.deserializeRow(t.rowSlot(i))
		PrintRow(&statement.insertRow)
	}
	return "success"
}


func (b *Buffer) ExecuteCmd(table *Table) {
	tokens := b.TokenizeInput()
	cmd := Cmd(tokens[0])
	if !b.ExistCmd(cmd){
		fmt.Printf("{%s} command doesn't exist yet :{\n", cmd)
		return
	}

	statement :=  newStatement()
	switch cmd {

	case EXIT:
		os.Exit(0)

	case INSERT:
		if len(tokens) != 4 {
			return 
		}

		statement.rowType = INSERT.String()
		statement.prepareStatement(tokens[1:])
		table.ExecuteInsert(statement)

	case SELECT:
		statement.rowType = SELECT.String()
		table.ExecuteSelect(*statement)
	}
}
