package main

import "fmt"

// @RouteType
type Vertex struct {
	X int
	Y int
}

// @Route(/api/login, post, Vertex)
func main() {
	// @Route(/api/register, post)
	fmt.Println("Hello, World!");
}