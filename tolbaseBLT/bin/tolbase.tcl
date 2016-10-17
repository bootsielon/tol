# this script is needed to start tolbase on Linux in non-interactive mode
lappend auto_path file normalize [file join [ file dir [info script] ] .. lib ] ]
package require TolbaseApp
