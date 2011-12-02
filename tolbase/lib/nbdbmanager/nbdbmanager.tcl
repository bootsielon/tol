package require BWidget
package require snit
package require mimetex
package require notebookdb
package require trycatch

if { [ info commands "try" ] eq "" } {
  namespace import ::trycatch::*
}


snit::type nbdbmanagerType {

  #-------------------------------------------------------------------
  # Instance Variables
  
  # This is an array, indexed by normalized notebook filename.
  # The elements are lists: db <notebookdb> ref <refcount>
  variable registry

  method create { name } {
    # FIRST, put the file name into canonical form as an index key.
    set canonicalName [file normalize $name]
    
    # NEXT, do we have a notebookdb for this name?
    if {[info exists registry($canonicalName)]} {
      
      # We do.  Increment its reference count.
      array set data $registry($canonicalName)
      incr data(ref)
      set registry($canonicalName) [array get data]
      
      # Return the object.
      return $data(db)
    }
    # NEXT, try to open a file with this name.  It might fail with
    # an error, which we won't bother catching; we'd just need to
    # rethrow it.
    set data(db) [ ::notebookdb::notebookdb create %AUTO% -dbfile $name ]
    
    # NEXT, it succeeded.  Add it to the registry with a reference
    # count of 1.
    set data(ref) 1
    set registry($canonicalName) [array get data]
    
    # NEXT, define the Notebook Commands and the required pages
    $self InitializeCommands $data(db)
    
    # FINALLY, return the new notebookdb.
    return $data(db) 
  }

  method InitializeCommands { db } {
    # FIRST, define a private namespace for Notebook's use
    $db eval "namespace eval ::__notebook:: { }"
    
    # FIRST, alias "kernel" commands defined elsewhere in the
    # application.
    $db alias parse             ::markupparser::parse
    $db alias image             image
    $db alias GetImage          Bitmap::get
    $db alias mimetexRender     mimetex::RenderImage
    $db eval {
      ::db::PageExpander evalcmd escapemime
      ::db::PageExpander errmode error
      
      proc escapemime { macro } {
        #puts "escapeeval = '$macro'"
        if { [ string range [ string trimleft $macro ] 0 6 ] eq "mimetex" } {
          set latex [ string range $macro 7 end ]
          return [ mimetex $latex ]
        } else {
          eval $macro
        }
      }
      
      proc img { imageORfile } {
        if { [ lsearch [ image names ]  $imageORfile ] == -1 } {
          set img [ GetImage $imageORfile ]
        } else {
          set img $imageORfile
        }
        return "\[!image $img !\]"
      }
      
      proc mimetex { latex } {
        set img [ mimetexRender $latex ] 
        return "\[!image $img !\]"
      }
      
      array set urlConf {
        tol,root https://www.tol-project.org
        tol,wiki https://www.tol-project.org/wiki
        tol,milestone https://www.tol-project.org/milestone
        tol,ticket https://www.tol-project.org/ticket
        mms,root https://trac.localbayes.es/mms
        mms,wiki https://trac.localbayes.es/mms/wiki
        mms,milestone https://www.tol-project.org/mms/milestone
        mms,ticket https://www.tol-project.org/mms/ticket
      }
      
      # [@url mms howto/5 "HOWTO 5" wiki @]
      # [@url mms howto/5 "HOWTO 5" @]
      # [@url mms howto/5 @]
      proc url { where path args } {
        foreach { label section } {"" ""} break
        foreach { label section } $args break
        if { $section eq "" } {
          set section "wiki"
        }
        set urlroot $::urlConf(${where},root)
        set url "$::urlConf(${where},root)/$section/$path"
        if { $label eq "" } {
          return "\[${url}\]"
        } else {
          return "\[$label | ${url}\]"
        }
      }

      # [@mms howto/5 "HOWTO 5" wiki @]
      # [@mms howto/5 "HOWTO 5" @]
      # [@mms howto/5 @]
      proc mms { path args } {
        return [ eval url mms $path $args ]
      }

      proc tol { path args } {
        return [ eval url tol $path $args ]
      }
    }
  }
}

package provide nbdbmanager 1.0
