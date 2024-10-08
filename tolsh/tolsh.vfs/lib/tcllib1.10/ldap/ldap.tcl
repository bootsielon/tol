#-----------------------------------------------------------------------------
#   Copyright (C) 1999-2004 Jochen C. Loewer (loewerj@web.de)
#   Copyright (C) 2006      Michael Schlenker (mic42@users.sourceforge.net)    
#-----------------------------------------------------------------------------
#
#   A (partial) LDAPv3 protocol implementation in plain Tcl.
#
#   See RFC 2251 and ASN.1 (X.680) and BER (X.690).
#
#
#   This software is copyrighted by Jochen C. Loewer (loewerj@web.de). The
#   following terms apply to all files associated with the software unless
#   explicitly disclaimed in individual files.
#
#   The authors hereby grant permission to use, copy, modify, distribute,
#   and license this software and its documentation for any purpose, provided
#   that existing copyright notices are retained in all copies and that this
#   notice is included verbatim in any distributions. No written agreement,
#   license, or royalty fee is required for any of the authorized uses.
#   Modifications to this software may be copyrighted by their authors
#   and need not follow the licensing terms described here, provided that
#   the new terms are clearly indicated on the first page of each file where
#   they apply.
#
#   IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
#   FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
#   ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
#   DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
#   POSSIBILITY OF SUCH DAMAGE.
#
#   THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
#   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
#   IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
#   NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
#   MODIFICATIONS.
#
#   $Id: ldap.tcl,v 1.23 2006/11/15 19:28:03 mic42 Exp $
#
#   written by Jochen Loewer
#   3 June, 1999
#
#-----------------------------------------------------------------------------

package require Tcl 8.4
package require asn 0.7
package provide ldap 1.6.8

namespace eval ldap {

    namespace export    connect secure_connect  \
                        disconnect              \
                        bind unbind             \
                        bindSASL                \
                        search                  \
                        searchInit           	\
		        searchNext	        \
		        searchEnd		\
                        modify                  \
                        modifyMulti             \
                        add                     \
		        addMulti		\
                        delete                  \
                        modifyDN		\
		        info

    namespace import ::asn::*
    
    variable SSLCertifiedAuthoritiesFile
    variable doDebug

    set doDebug 0
   
    # LDAP result codes from the RFC
    variable resultCode2String
    array set resultCode2String {
         0  success
         1  operationsError
         2  protocolError
         3  timeLimitExceeded
         4  sizeLimitExceeded
         5  compareFalse
         6  compareTrue
         7  authMethodNotSupported
         8  strongAuthRequired
        10  referral
        11  adminLimitExceeded
        12  unavailableCriticalExtension
        13  confidentialityRequired
        14  saslBindInProgress
        16  noSuchAttribute
        17  undefinedAttributeType
        18  inappropriateMatching
        19  constraintViolation
        20  attributeOrValueExists
        21  invalidAttributeSyntax
        32  noSuchObject
        33  aliasProblem
        34  invalidDNSyntax
        35  isLeaf
        36  aliasDereferencingProblem
        48  inappropriateAuthentication
        49  invalidCredentials
        50  insufficientAccessRights
        51  busy
        52  unavailable
        53  unwillingToPerform
        54  loopDetect
        64  namingViolation
        65  objectClassViolation
        66  notAllowedOnNonLeaf
        67  notAllowedOnRDN
        68  entryAlreadyExists
        69  objectClassModsProhibited
        80  other
    }
    
}


#-----------------------------------------------------------------------------
#    Lookup an numerical ldap result code and return a string version
#
#-----------------------------------------------------------------------------
proc ::ldap::resultCode2String {code} {
    variable resultCode2String
    if {[::info exists resultCode2String($code)]} {
	    return $resultCode2String($code)
    } else {
	    return "unknownError"
    }
}

#-----------------------------------------------------------------------------
#   Basic sanity check for connection handles
#   must be an array
#-----------------------------------------------------------------------------
proc ::ldap::CheckHandle {handle} {
    if {![array exists $handle]} {
        return -code error \
            [format "Not a valid LDAP connection handle: %s" $handle]
    }
}

#-----------------------------------------------------------------------------
#    info
#
#-----------------------------------------------------------------------------

proc ldap::info {args} {
   set cmd [lindex $args 0]
   set cmds {connections bound bounduser control extensions features ip saslmechanisms tls whoami}
   if {[llength $args] == 0} {
   	return -code error \
		"Usage: \"info subcommand ?handle?\""    
   }
   if {[lsearch -exact $cmds $cmd] == -1} {
   	return -code error \
		"Invalid subcommand \"$cmd\", valid commands are\
		[join [lrange $cmds 0 end-1] ,] and [lindex $cmds end]" 
   }
   eval [linsert [lrange $args 1 end] 0 ldap::info_$cmd]    
}

#-----------------------------------------------------------------------------
#    get the ip address of the server we connected to
# 
#-----------------------------------------------------------------------------
proc ldap::info_ip {args} {
   if {[llength $args] != 1} {
   	return -code error \
	       "Wrong # of arguments. Usage: ldap::info ip handle"
   }
   CheckHandle [lindex $args 0]
   upvar #0 [lindex $args 0] conn
   if {![::info exists conn(sock)]} {
   	return -code error \
		"\"[lindex $args 0]\" is not a ldap connection handle"
   }
   return [lindex [fconfigure $conn(sock) -peername] 0]
}

#-----------------------------------------------------------------------------
#   get the list of open ldap connections
#
#-----------------------------------------------------------------------------
proc ldap::info_connections {args} {
   if {[llength $args] != 0} {
   	return -code error \
	       "Wrong # of arguments. Usage: ldap::info connections"   
   }
   return [::info vars ::ldap::ldap*]
}

#-----------------------------------------------------------------------------
#   check if the connection is bound
#
#-----------------------------------------------------------------------------
proc ldap::info_bound {args} {
   if {[llength $args] != 1} {
   	return -code error \
	       "Wrong # of arguments. Usage: ldap::info bound handle"
   }
   CheckHandle [lindex $args 0]
   upvar #0 [lindex $args 0] conn
   if {![::info exists conn(bound)]} {
   	return -code error \
		"\"[lindex $args 0]\" is not a ldap connection handle"
   }
   
   return $conn(bound)
}

#-----------------------------------------------------------------------------
#   check with which user the connection is bound
#
#-----------------------------------------------------------------------------
proc ldap::info_bounduser {args} {
   if {[llength $args] != 1} {
   	return -code error \
	       "Wrong # of arguments. Usage: ldap::info bounduser handle"
   }
   CheckHandle [lindex $args 0]   
   upvar #0 [lindex $args 0] conn
   if {![::info exists conn(bound)]} {
   	return -code error \
		"\"[lindex $args 0]\" is not a ldap connection handle"
   }
   
   return $conn(bounduser)
}

#-----------------------------------------------------------------------------
#   check if the connection uses tls
#
#-----------------------------------------------------------------------------

proc ldap::info_tls {args} {
   if {[llength $args] != 1} {
   	return -code error \
	       "Wrong # of arguments. Usage: ldap::info tls handle"
   }
   CheckHandle [lindex $args 0]   
   upvar #0 [lindex $args 0] conn
   if {![::info exists conn(tls)]} {
   	return -code error \
		"\"[lindex $args 0]\" is not a ldap connection handle"
   }
   return $conn(tls)
}

proc ldap::info_saslmechanisms {args} {
   if {[llength $args] != 1} {
   	return -code error \
	       "Wrong # of arguments. Usage: ldap::info saslmechanisms handle"
   }
   return [Saslmechanisms [lindex $args 0]]
}

proc ldap::info_extensions {args} {
   if {[llength $args] != 1} {
   	return -code error \
	       "Wrong # of arguments. Usage: ldap::info extensions handle"
   }
   return [Extensions [lindex $args 0]]
}

proc ldap::info_control {args} {
   if {[llength $args] != 1} {
   	return -code error \
	       "Wrong # of arguments. Usage: ldap::info control handle"
   }
   return [Control [lindex $args 0]]
}

proc ldap::info_features {args} {
   if {[llength $args] != 1} {
   	return -code error \
	       "Wrong # of arguments. Usage: ldap::info features handle"
   }
   return [Features [lindex $args 0]]
}

proc ldap::info_whoami {args} {
   if {[llength $args] != 1} {
   	return -code error \
	       "Wrong # of arguments. Usage: ldap::info whoami handle"
   }
   return [Whoami [lindex $args 0]]
}


#-----------------------------------------------------------------------------
# Basic server introspection support
#
#-----------------------------------------------------------------------------
proc ldap::Saslmechanisms {conn} {
    CheckHandle $conn
    lindex [ldap::search $conn {} {(objectClass=*)} \
                    {supportedSASLMechanisms} -scope base] 0 1 1
}

proc ldap::Extensions {conn} {
    CheckHandle $conn
    lindex [ldap::search $conn {} {(objectClass=*)} \
                    {supportedExtension} -scope base] 0 1 1
}

proc ldap::Control {conn} {
    CheckHandle $conn
    lindex [ldap::search $conn {} {(objectClass=*)} \
                    {supportedControl} -scope base] 0 1 1
}

proc ldap::Features {conn} {
    CheckHandle $conn
    lindex [ldap::search $conn {} {(objectClass=*)} \
                    {supportedFeatures} -scope base] 0 1 1
}

#-------------------------------------------------------------------------------
# Implements the RFC 4532 extension "Who am I?"
#
#-------------------------------------------------------------------------------
proc ldap::Whoami {handle} {
    CheckHandle $handle
    if {[lsearch [ldap::Extensions $handle] 1.3.6.1.4.1.4203.1.11.3] == -1} {
        return -code error \
            "Server does not support the \"Who am I?\" extension"
    }
    
    set request [asnApplicationConstr 23 [asnOctetString 1.3.6.1.4.1.4203.1.11.3]]
    set mid [SendMessage $handle $request]
    set response [WaitForResponse $handle $mid]
 
    asnGetApplication response appNum
    if {$appNum != 24} {
        return -code error \
             "unexpected application number ($appNum != 24)"        
    }
    
    asnGetEnumeration response resultCode
    asnGetOctetString response matchedDN
    asnGetOctetString response errorMessage
    if {$resultCode != 0} {
        return -code error \
		-errorcode [list LDAP [resultCode2String $resultCode] $matchedDN $errorMessage] \
		"LDAP error [resultCode2String $resultCode] '$matchedDN': $errorMessage"
    }
    set whoami ""
    if {[string length $response]} {
        asnRetag response 0x04
        asnGetOctetString response whoami
    }
    return $whoami
}

#-----------------------------------------------------------------------------
#    connect
#
#-----------------------------------------------------------------------------
proc ldap::connect { host {port 389} } {

    #--------------------------------------
    #   connect via TCP/IP
    #--------------------------------------
    set sock [socket $host $port]
    fconfigure $sock -blocking no -translation binary -buffering full

    #--------------------------------------
    #   initialize connection array
    #--------------------------------------
    upvar #0 ::ldap::ldap$sock conn
    catch { unset conn }

    set conn(host)      $host
    set conn(sock)      $sock
    set conn(messageId) 0
    set conn(tls)       0
    set conn(bound)     0
    set conn(bounduser) ""
    set conn(saslBindInProgress) 0
    set conn(tlsHandshakeInProgress) 0
    set conn(lastError) ""
    
    fileevent $sock readable [list ::ldap::MessageReceiver ::ldap::ldap$sock]
    return ::ldap::ldap$sock
}

#-----------------------------------------------------------------------------
#    secure_connect
#
#-----------------------------------------------------------------------------
proc ldap::secure_connect { host {port 636} } {

    variable SSLCertifiedAuthoritiesFile

    package require tls

    #------------------------------------------------------------------
    #   connect via TCP/IP
    #------------------------------------------------------------------
    set sock [socket $host $port]
    fconfigure $sock -blocking no -translation binary -buffering full

    #------------------------------------------------------------------
    #   make it a SSL connection
    #
    #------------------------------------------------------------------
    #tls::import $sock -cafile $SSLCertifiedAuthoritiesFile -ssl2 no -ssl3 yes -tls1 yes
    tls::import $sock -cafile "" -certfile "" -keyfile "" \
                      -request 1 -server 0 -require 0 -ssl2 no -ssl3 yes -tls1 yes
    set retry 0
    while {1} {
        if {$retry > 20} {
            close $sock
            return -code error "too long retry to setup SSL connection"
        }
        if {[catch { tls::handshake $sock } err]} {
            if {[string match "*resource temporarily unavailable*" $err]} {
                after 50
                incr retry
            } else {
                close $sock
                return -code error $err
            }
        } else {
            break
        }
    }

    #--------------------------------------
    #   initialize connection array
    #--------------------------------------
    upvar ::ldap::ldap$sock conn
    catch { unset conn }

    set conn(host)      $host
    set conn(sock)      $sock
    set conn(messageId) 0
    set conn(tls)       1
    set conn(bound)     0
    set conn(bounduser) ""
    set conn(saslBindInProgress) 0
    set conn(tlsHandshakeInProgress) 0
    set conn(lasterror) ""
    
    fileevent $sock readable [list ::ldap::MessageReceiver ::ldap::ldap$sock]
    return ::ldap::ldap$sock
}


#------------------------------------------------------------------------------
#    starttls -  negotiate tls on an open ldap connection
#
#------------------------------------------------------------------------------
proc ldap::starttls {handle {cafile ""} {certfile ""} {keyfile ""}} {
    CheckHandle $handle

    upvar #0 $handle conn
    
    if {$conn(tls)} {
        return -code error \
            "Cannot StartTLS on connection, TLS already running"
    }
    
    if {[ldap::waitingForMessages $handle]} {
        return -code error \
            "Cannot StartTLS while waiting for repsonses"
    }
    
    if {$conn(saslBindInProgress)} {
        return -code error \
            "Cannot StartTLS while SASL bind in progress"
    }
    
    if {[lsearch -exact [ldap::Extensions $handle] 1.3.6.1.4.1.1466.20037] == -1} {
        return -code error \
            "Server does not support the StartTLS extension"
    }
    package require tls
    
    
    set request [asnApplicationConstr 23 [asnOctetString 1.3.6.1.4.1.1466.20037]]
    set mid [SendMessage $handle $request]
    set conn(tlsHandshakeInProgress) 1
    set response [WaitForResponse $handle $mid]
 
    asnGetApplication response appNum
    if {$appNum != 24} {
        set conn(tlsHandshakeInProgress) 0
        return -code error \
             "unexpected application number ($appNum != 24)"        
    }
    
    asnGetEnumeration response resultCode
    asnGetOctetString response matchedDN
    asnGetOctetString response errorMessage
    if {$resultCode != 0} {
        set conn(tlsHandshakeInProgress) 0
        return -code error \
		-errorcode [list LDAP [resultCode2String $resultCode] $matchedDN $errorMessage] \
		"LDAP error [resultCode2String $resultCode] '$matchedDN': $errorMessage"
    }
    set oid "1.3.6.1.4.1.1466.20037"
    if {[string length $response]} {
        asnRetag response 0x04
        asnGetOctetString response oid
    }
    if {$oid ne "1.3.6.1.4.1.1466.20037"} {
        set conn(tlsHandshakeInProgress) 0
        return -code error \
            "Unexpected LDAP response"
    } 

    tls::import $conn(sock) -cafile $cafile -certfile $certfile -keyfile $keyfile \
                      -request 1 -server 0 -require 0 -ssl2 no -ssl3 yes -tls1 yes
    set retry 0
    while {1} {
        if {$retry > 20} {
            close $sock
            return -code error "too long retry to setup SSL connection"
        }
        if {[catch { tls::handshake $conn(sock) } err]} {
            if {[string match "*resource temporarily unavailable*" $err]} {
                after 50
                incr retry
            } else {
                close $conn(sock)
                return -code error $err
            }
        } else {
            break
        }
    }
    set conn(tls) 1
    set conn(tlsHandshakeInProgress) 0
    return 1
}



#------------------------------------------------------------------------------
#  Create a new unique message and send it over the socket.
#
#------------------------------------------------------------------------------

proc ldap::CreateAndSendMessage {handle payload} {
    upvar #0 $handle conn
    
    if {$conn(tlsHandshakeInProgress)} {
        return -code error \
            "Cannot send other LDAP PDU while TLS handshake in progress"
    }
    
    incr conn(messageId)
    set message [asnSequence [asnInteger $conn(messageId)] $payload]
    debugData "Message $conn(messageId) Sent" $message
    puts -nonewline $conn(sock) $message
    flush $conn(sock)
    return $conn(messageId)
}

#------------------------------------------------------------------------------
#  Send a message to the server which expects a response,
#  returns the messageId which is to be used with FinalizeMessage 
#  and WaitForResponse
#
#------------------------------------------------------------------------------
proc ldap::SendMessage {handle pdu} {
    upvar #0 $handle conn
    set mid [CreateAndSendMessage $handle $pdu] 
    
    # safe the state to match responses   
    set conn(message,$mid) [list]
    return $mid                
}

#------------------------------------------------------------------------------
#  Send a message to the server without expecting a response
#
#------------------------------------------------------------------------------
proc ldap::SendMessageNoReply {handle pdu} {
    upvar #0 $handle conn
    return [CreateAndSendMessage $handle $pdu]                
}

#------------------------------------------------------------------------------
# Cleanup the storage associated with a messageId
#
#------------------------------------------------------------------------------
proc ldap::FinalizeMessage {handle messageId} {
    upvar #0 $handle conn
    trace "Message $messageId finalized"
    unset -nocomplain conn(message,$messageId)
}

#------------------------------------------------------------------------------
#  Wait for a response for the given messageId.
#
#  This waits in a vwait if no message has yet been received or returns
#  the oldest message at once, if it is queued.
#
#------------------------------------------------------------------------------
proc ldap::WaitForResponse {handle messageId} {
    upvar #0 $handle conn
    
    trace "Waiting for Message $messageId"
    # check if the message waits for a reply
    if {![::info exists conn(message,$messageId)]} {
        return -code error \
            [format "Cannot wait for message %d." $messageId]
    }
    
    # check if we have a received response in the buffer
    if {[llength $conn(message,$messageId)] > 0} {
        set response [lindex $conn(message,$messageId) 0]
        set conn(message,$messageId) [lrange $conn(message,$messageId) 1 end]
        return $response
    }
    
    # wait for an incoming response
    vwait [namespace which -variable $handle](message,$messageId)
    if {[llength $conn(message,$messageId)] == 0} {
        # We have waited and have been awakended but no message is there
        if {[string length $conn(lastError)]} {
            return -code error \
                [format "Protocol error: %s" $conn(lastError)]
        } else {
            return -code error \
                [format "Broken response for message %d" $messageId]
        }
    }
    set response [lindex $conn(message,$messageId) 0]
    set conn(message,$messageId) [lrange $conn(message,$messageId) 1 end]
    return $response        
}

proc ldap::waitingForMessages {handle} {
    upvar #0 $handle conn
    return [llength [array names conn message,*]]
}

#------------------------------------------------------------------------------
# Process a single response PDU. Decodes the messageId and puts the
# message into the appropriate queue.
#
#------------------------------------------------------------------------------

proc ldap::ProcessMessage {handle response} {
    upvar #0 $handle conn

    # decode the messageId
    asnGetInteger  response messageId
    
    # check if we wait for a response
    if {[::info exists conn(message,$messageId)]} {
        # append the new message, which triggers 
        # message handlers using vwait on the entry
        lappend conn(message,$messageId) $response
        return
    }
    
    # handle unsolicited server responses
    
    if {0} {
        asnGetApplication response appNum
        #if { $appNum != 24 } {
        #     error "unexpected application number ($appNum != 24)"
        #}
        asnGetEnumeration response resultCode
        asnGetOctetString response matchedDN
        asnGetOctetString response errorMessage
        if {[string length $response]} {
            asnGetOctetString response responseName
        }
        if {[string length $response]} {
            asnGetOctetString response responseValue
        }
        if {$resultCode != 0} {
            return -code error \
		    -errorcode [list LDAP [resultCode2String $resultCode] $matchedDN $errorMessage] \
		    "LDAP error [resultCode2String $resultCode] '$matchedDN': $errorMessage"    
        }
    }
    #dumpASN1Parse $response
    #error "Unsolicited message from server"
    
}

#-------------------------------------------------------------------------------
# Get the code out of waitForResponse in case of errors
#
#-------------------------------------------------------------------------------
proc ldap::CleanupWaitingMessages {handle} {
    upvar #0 $handle conn
    foreach message [array names conn message,*] {
        set conn($message) [list]
    }
}

#-------------------------------------------------------------------------------
#  The basic fileevent based message receiver.
#  It reads PDU's from the network in a non-blocking fashion.
#
#-------------------------------------------------------------------------------
proc ldap::MessageReceiver {handle} {
    upvar #0 $handle conn
    
    # We have to account for partial PDUs received, so
    # we keep some state information.
    #
    #   conn(pdu,partial)  -- we are reading a partial pdu if non zero
    #   conn(pdu,length_bytes) -- the buffer for loading the length
    #   conn(pdu,length)   -- we have decoded the length if >= 0, if <0 it contains 
    #                         the length of the length encoding in bytes
    #   conn(pdu,payload)  -- the payload buffer
    #   conn(pdu,received) -- the data received
    
    # fetch the sequence byte
    if {[::info exists conn(pdu,partial)] && $conn(pdu,partial) != 0} {
        # we have decoded at least the type byte    
    } else {
        foreach {code type} [ReceiveBytes $conn(sock) 1] {break}
        switch -- $code {
            ok {
                binary scan $type c byte
                set type [expr {($byte + 0x100) % 0x100}]  
                if {$type != 0x30} {
                    CleanupWaitingMessages $handle
                    set conn(lastError) [format "Expected SEQUENCE (0x30) but got %x" $type]
                    return
                } else {
                    set conn(pdu,partial) 1
                    append conn(pdu,received) $type
                }
                }
            eof {
                CleanupWaitingMessages $handle
                set conn(lastError) "Server closed connection"
                catch {close $conn(sock)}
                return
            } 
            default {
                CleanupWaitingMessages $handle
                set bytes $type[read $conn(sock)]
                binary scan $bytes h* values
                set conn(lastError) [format \
                    "Error reading SEQUENCE response for handle %s : %s : %s" $handle $code $values]
                return
                }
        }
    }
    
    
    # fetch the length
    if {[::info exists conn(pdu,length)] && $conn(pdu,length) >= 0} {
        # we already have a decoded length
    } else {
        if {[::info exists conn(pdu,length)] && $conn(pdu,length) < 0} {
            # we already know the length, but have not received enough bytes to decode it
            set missing [expr {1+abs($conn(pdu,length))-[string length $conn(pdu,length_bytes)]}]
            if {$missing != 0} {
                foreach {code bytes} [ReceiveBytes $conn(sock) $missing] {break}
                switch -- $code {
                    "ok"  {
                        append conn(pdu,length_bytes) $bytes
                        append conn(pdu,received) $bytes
                        asnGetLength conn(pdu,length_bytes) conn(pdu,length)
                    }
                    "partial" {
                        append conn(pdu,length_bytes) $bytes
                        append conn(pdu,received) $bytes
                        return
                    }
                    "eof" {
                        CleanupWaitingMessages $handle            
                        catch {close $conn(sock)}
                        set conn(lastError) "Server closed connection"
                        return
                    } 
                    default {
                        CleanupWaitingMessages $handle            
                        set conn(lastError) [format \
                            "Error reading LENGTH2 response for handle %s : %s" $handle $code]
                        return
                    }
                }
            }
        } else {
            # we know nothing, need to read the first length byte
            foreach {code bytes} [ReceiveBytes $conn(sock) 1] {break}
            switch -- $code {
                "ok"  {
                    set conn(pdu,length_bytes) $bytes
                    binary scan $bytes c byte
                    set size [expr {($byte + 0x100) % 0x100}]  
                    if {$size > 0x080} {
                        set conn(pdu,length) [expr {-1* ($size & 0x7f)}]
                        # fetch the rest with the next fileevent
                        return 
                    } else {
                        asnGetLength conn(pdu,length_bytes) conn(pdu,length)
                    }
                }
                "eof" {
                    CleanupWaitingMessages $handle            
                    catch {close $conn(sock)}
                    set conn(lastError) "Server closed connection"
                }                 
                default {
                    CleanupWaitingMessages $handle            
                    set conn(lastError) [format \
                        "Error reading LENGTH1 response for handle %s : %s" $handle $code]
                    return
                }       
            }
        }
    }
    
    if {[::info exists conn(pdu,payload)]} {
        # length is decoded, we can read the rest
        set missing [expr {$conn(pdu,length) - [string length $conn(pdu,payload)]}]
    } else {
        set missing $conn(pdu,length)
    }
    if {$missing > 0} {
        foreach {code bytes} [ReceiveBytes $conn(sock) $missing] {break}
        switch -- $code {
            "ok" {
                append conn(pdu,payload) $bytes
            }
            "partial" {
                append conn(pdu,payload) $bytes
                return
            }
            "eof" {
                CleanupWaitingMessages $handle            
                catch {close $conn(sock)}
                set conn(lastError) "Server closed connection"
            }             
            default {
                CleanupWaitingMessages $handle            
                set conn(lastError) [format \
                    "Error reading DATA response for handle %s : %s" $handle $code]
                return
            }
        }
    }
    
    # we have a complete PDU, push it for processing
    set pdu $conn(pdu,payload)
    set conn(pdu,payload) ""
    set conn(pdu,partial) 0
    unset -nocomplain set conn(pdu,length) 
    set conn(pdu,length_bytes) ""    
   
    # reschedule message Processing
    after 0 [list ::ldap::ProcessMessage $handle $pdu]
}

#-------------------------------------------------------------------------------
# Receive the number of bytes from the socket and signal error conditions.
#
#-------------------------------------------------------------------------------
proc ldap::ReceiveBytes {sock bytes} {
    set status [catch {read $sock $bytes} block]
    if { $status != 0 } {
        return [list error $block]
    } elseif { [string length $block] == $bytes } {
        # we have all bytes we wanted
        return [list ok $block]
    } elseif { [eof $sock] } {
        return [list eof $block]
    } elseif { [fblocked $sock] || ([string length $block] < $bytes)} {
        return [list partial $block]
    } else {
        error "Socket state for socket $sock undefined!" 
    }  
}

#-----------------------------------------------------------------------------
#    bindSASL  -  does a bind with SASL authentication
#-----------------------------------------------------------------------------

proc ldap::bindSASL {handle {name ""} {password ""} } {
    CheckHandle $handle

    package require SASL
    
    upvar #0 $handle conn
    
    set mechs [ldap::Saslmechanisms $handle]
    
    set conn(saslBindInProgress) 1    
    set auth 0
    foreach mech [SASL::mechanisms] {
        if {[lsearch -exact $mechs $mech] == -1} { continue }
        trace "Using $mech for SASL Auth"
        if {[catch {
            SASLAuth $handle $mech $name $password
        } msg]} {
            trace [format "AUTH %s failed: %s" $mech $msg]
        } else {
	   # AUTH was successful 
	   if {$msg == 1} {
	       set auth 1
	       break
	   }
	}    
    }        
    
    set conn(saslBindInProgress) 0
    return $auth
}

#-----------------------------------------------------------------------------
#    SASLCallback - Callback to use for SASL authentication
#
#    More or less cut and copied from the smtp module.
#    May need adjustments for ldap.
#
#-----------------------------------------------------------------------------
proc ::ldap::SASLCallback {handle context command args} {
    upvar #0 $handle conn
    upvar #0 $context ctx
    array set options $conn(options)
    trace "SASLCallback $command"
    switch -exact -- $command {
        login    { return $options(-username) }
        username { return $options(-username) }
        password { return $options(-password) }
        hostname { return [::info hostname] }
        realm    { 
            if {[string equal $ctx(mech) "NTLM"] \
                    && [info exists ::env(USERDOMAIN)]} {
                return $::env(USERDOMAIN)
            } else {
                return ""
            }
        }
        default  { 
            return -code error "error: unsupported SASL information requested"
        }
    }
}

#-----------------------------------------------------------------------------
#    SASLAuth - Handles the actual SASL message exchange
#
#-----------------------------------------------------------------------------

proc ldap::SASLAuth {handle mech name password} {
    upvar 1 $handle conn
    
    set conn(options) [list -password $password -username $name]

    # check for tcllib bug # 1545306 and reset the nonce-count if 
    # found, so a second call to this code does not fail
    #
    if {[::info exists ::SASL::digest_md5_noncecount]} {
        set ::SASL::digest_md5_noncecount 0
    }
    
    set ctx [SASL::new -mechanism $mech \
                       -service ldap    \
                       -callback [list ::ldap::SASLCallback $handle]]

    set msg(serverSASLCreds) ""
    # Do the SASL Message exchanges
    while {[SASL::step $ctx $msg(serverSASLCreds)]} {
        # Create and send the BindRequest
        set request [buildSASLBindRequest "" $mech [SASL::response $ctx]]
        set messageId [SendMessage $handle $request]
        debugData bindRequest $request
        
        set response [WaitForResponse $handle $messageId]
        FinalizeMessage $handle $messageId
        debugData bindResponse $response
        
        array set msg [decodeSASLBindResponse $handle $response]
        
	# Check for Bind success
        if {$msg(resultCode) == 0} { 
            set conn(bound) 1
            set conn(bounduser) $name
            SASL::cleanup $ctx
            break        
        }
        
	# Check if next SASL step is requested
        if {$msg(resultCode) == 14} {
            continue
        }
	
        SASL::cleanup $ctx
        # Something went wrong
        return 	-code error \
		-errorcode [list LDAP [resultCode2String $msg(resultCode)] \
				 $msg(matchedDN) $msg(errorMessage)] \
		"LDAP error [resultCode2String $msg(resultCode)] '$msg(matchedDN)': $msg(errorMessage)"
    }
    
    return 1
}

#----------------------------------------------------------------------------
#
# Create a LDAP BindRequest using SASL
#
#----------------------------------------------------------------------------

proc ldap::buildSASLBindRequest {name mech {credentials {}}} {
    if {$credentials ne {}} {
       set request [  asnApplicationConstr 0            		\
            [asnInteger 3]                 		\
            [asnOctetString $name]         		\
            [asnChoiceConstr 3                   	\
                    [asnOctetString $mech]      	\
                    [asnOctetString $credentials] 	\
            ]  \                            		                                      
        ] 
    } else {  
    set request [   asnApplicationConstr 0            		\
        [asnInteger 3]                 		\
        [asnOctetString $name]         		\
        [asnChoiceConstr 3                   	\
                [asnOctetString $mech]      	\
        ] \
        ]                              		                                      
    }
    return $request
}

#-------------------------------------------------------------------------------
#
# Decode an LDAP BindResponse
#
#-------------------------------------------------------------------------------
proc ldap::decodeSASLBindResponse {handle response} {
    upvar #0 $handle conn

    asnGetApplication response appNum
    if { $appNum != 1 } {
        error "unexpected application number ($appNum != 1)"
    }
    asnGetEnumeration response resultCode
    asnGetOctetString response matchedDN
    asnGetOctetString response errorMessage

    # Check if we have a serverSASLCreds field left,
    # or if this is a simple response without it
    # probably an error message then.
    if {[string length $response]} {
        asnRetag response 0x04
        asnGetOctetString response serverSASLCreds
    } else {
        set serverSASLCreds ""
    } 
    return [list appNum $appNum \
                 resultCode $resultCode matchedDN $matchedDN \
                 errorMessage $errorMessage serverSASLCreds $serverSASLCreds]
}


#-----------------------------------------------------------------------------
#    bind  -  does a bind with simple authentication
#
#-----------------------------------------------------------------------------
proc ldap::bind { handle {name ""} {password ""} } {
    CheckHandle $handle
    
    upvar #0 $handle conn

    #-----------------------------------------------------------------
    #   marshal bind request packet and send it
    #
    #-----------------------------------------------------------------
    set request [asnApplicationConstr 0                \
                        [asnInteger 3]                 \
                        [asnOctetString $name]         \
                        [asnChoice 0 $password]        \
                ]                                  
    set messageId [SendMessage $handle $request]            
    debugData bindRequest $request
    
    set response [WaitForResponse $handle $messageId]
    FinalizeMessage $handle $messageId
    debugData bindResponse $response
    
    asnGetApplication response appNum
    if { $appNum != 1 } {
        error "unexpected application number ($appNum != 1)"
    }
    asnGetEnumeration response resultCode
    asnGetOctetString response matchedDN
    asnGetOctetString response errorMessage
    if {$resultCode != 0} {
        return -code error \
		-errorcode [list LDAP [resultCode2String $resultCode] $matchedDN $errorMessage] \
		"LDAP error [resultCode2String $resultCode] '$matchedDN': $errorMessage"
    }
    set conn(bound) 1
    set conn(bounduser) $name
}


#-----------------------------------------------------------------------------
#    unbind
#
#-----------------------------------------------------------------------------
proc ldap::unbind { handle } {
    CheckHandle $handle

    upvar #0 $handle conn

    #------------------------------------------------
    #   marshal unbind request packet and send it
    #------------------------------------------------
    set request [asnApplication 2 ""]         
    SendMessageNoReply $handle $request
    
    set conn(bounduser) ""
    set conn(bound) 0
    close $conn(sock)
    set conn(sock) ""
}


#-----------------------------------------------------------------------------
#    buildUpFilter  -   parses the text representation of LDAP search
#                       filters and transforms it into the correct
#                       marshalled representation for the search request
#                       packet
#
#-----------------------------------------------------------------------------
proc ldap::buildUpFilter { filter } {

    set first [lindex $filter 0]
    set data ""
    switch -regexp -- $first {
        ^\\&$ {  #--- and -------------------------------------------
            foreach term [lrange $filter 1 end] {
                append data [buildUpFilter $term]
            }
            return [asnChoiceConstr 0 $data]
        }
        ^\\|$ {  #--- or --------------------------------------------
            foreach term [lrange $filter 1 end] {
                append data [buildUpFilter $term]
            }
            return [asnChoiceConstr 1 $data]
        }
        ^\\!$ {  #--- not -------------------------------------------
            return [asnChoiceConstr 2 [buildUpFilter [lindex $filter 1]]]
        }
        =\\*$ {  #--- present ---------------------------------------
            set endpos [expr {[string length $first] -3}]
            set attributetype [string range $first 0 $endpos]
            return [asnChoice 7 $attributetype]
        }
        ^[0-9A-z.]*~= {  #--- approxMatch --------------------------
            regexp {^([0-9A-z.]*)~=(.*)$} $first all attributetype value
            return [asnChoiceConstr 8 [asnOctetString $attributetype] \
                                      [asnOctetString $value]         ]
        }
        ^[0-9A-z.]*<= {  #--- lessOrEqual --------------------------
            regexp {^([0-9A-z.]*)<=(.*)$} $first all attributetype value
            return [asnChoiceConstr 6 [asnOctetString $attributetype] \
                                      [asnOctetString $value]         ]
        }
        ^[0-9A-z.]*>= {  #--- greaterOrEqual -----------------------
            regexp {^([0-9A-z.]*)>=(.*)$} $first all attributetype value
            return [asnChoiceConstr 5 [asnOctetString $attributetype] \
                                      [asnOctetString $value]         ]
        }
        ^[0-9A-z.]*=.*\\*.* {  #--- substrings -----------------
            regexp {^([0-9A-z.]*)=(.*)$} $first all attributetype value
            regsub -all {\*+} $value {*} value
            set value [split $value "*"]
            
            set firstsubstrtype 0       ;# initial
            set lastsubstrtype  2       ;# final
            if {[string equal [lindex $value 0] ""]} {
                set firstsubstrtype 1       ;# any
                set value [lreplace $value 0 0]
            }
            if {[string equal [lindex $value end] ""]} {
                set lastsubstrtype 1        ;# any
                set value [lreplace $value end end]
            }
        
            set n [llength $value]
        
            set i 1
            set l {}
            set substrtype 0            ;# initial
            foreach str $value {
            if {$i == 1 && $i == $n} {
                if {$firstsubstrtype == 0} {
                set substrtype 0    ;# initial
                } elseif {$lastsubstrtype == 2} {
                set substrtype 2    ;# final
                } else {
                set substrtype 1    ;# any
                }
            } elseif {$i == 1} {
                set substrtype $firstsubstrtype
            } elseif {$i == $n} {
                set substrtype $lastsubstrtype
            } else {
                set substrtype 1        ;# any
            }
            lappend l [asnChoice $substrtype $str]
            incr i
            }
            return [asnChoiceConstr 4 [asnOctetString $attributetype]     \
                      [asnSequenceFromList $l] ]
        }
        ^[0-9A-z.]*= {  #--- equal ---------------------------------
            regexp {^([0-9A-z.]*)=(.*)$} $first all attributetype value
            trace "equal: attributetype='$attributetype' value='$value'"
            return [asnChoiceConstr 3 [asnOctetString $attributetype] \
                                      [asnOctetString $value]         ]
        }
        default {
            return [buildUpFilter $first]
            #error "cant handle $first for filter part"
        }
    }
}

#-----------------------------------------------------------------------------
#    search  -  performs a LDAP search below the baseObject tree using a
#               complex LDAP search expression (like "|(cn=Linus*)(sn=Torvalds*)"
#               and returns all matching objects (DNs) with given attributes
#               (or all attributes if empty list is given) as list:
#
#  {dn1 { attr1 {val11 val12 ...} attr2 {val21 val22 ... } ... }} {dn2 { ... }} ...
#
#-----------------------------------------------------------------------------
proc ldap::search { handle baseObject filterString attributes args} {
    CheckHandle $handle

    upvar #0 $handle conn

    searchInit $handle $baseObject $filterString $attributes $args

    set results    {}
    set lastPacket 0
    while { !$lastPacket } {

	set r [searchNext $handle]
	if {[llength $r] > 0} then {
	    lappend results $r
	} else {
	    set lastPacket 1
	}
    }
    searchEnd $handle

    return $results
}
#-----------------------------------------------------------------------------
#    searchInProgress - checks if a search is in progress
#
#-----------------------------------------------------------------------------

proc ldap::searchInProgress {handle} {
   CheckHandle $handle
   upvar #0 $handle conn
   if {[::info exists conn(searchInProgress)]} {
   	return $conn(searchInProgress)
   } else {
       	return 0
   }	   
}

#-----------------------------------------------------------------------------
#    searchInit - initiates an LDAP search
#
#-----------------------------------------------------------------------------
proc ldap::searchInit { handle baseObject filterString attributes opt} {
    CheckHandle $handle

    upvar #0 $handle conn

    if {[searchInProgress $handle]} {
        return -code error \
            "Cannot start search. Already a search in progress for this handle."    
    }
    
    set scope        2
    set derefAliases 0
    set sizeLimit    0
    set timeLimit    0
    set attrsOnly    0

    foreach {key value} $opt {
        switch -- [string tolower $key] {
            -scope {
                switch -- $value {
                   base 		{ set scope 0 }
                   one - onelevel 	{ set scope 1 }
                   sub - subtree 	{ set scope 2 }
                   default {  }
                }
            }
	    -derefaliases {
		switch -- $value {
		    never 	{ set derefAliases 0 }
		    search 	{ set derefAliases 1 }
		    find 	{ set derefAliases 2 }
		    always 	{ set derefAliases 3 }
		    default { }
		}
	    }
	    -sizelimit {
		set sizeLimit $value
	    }
	    -timelimit {
		set timeLimit $value
	    }
	    -attrsonly {
		set attrsOnly $value
	    }
	    default {
		return -code error \
			"Invalid search option '$key'"
	    }
        }
    }
    
    set request [buildSearchRequest $baseObject $scope \
    			$derefAliases $sizeLimit $timeLimit $attrsOnly $filterString \
			$attributes]
    set messageId [SendMessage $handle $request]
    debugData searchRequest $request
    
    # Keep the message Id, so we know about the search
    set conn(searchInProgress) $messageId

    return $conn(searchInProgress)
}

proc ldap::buildSearchRequest {baseObject scope derefAliases
    			       sizeLimit timeLimit attrsOnly filterString
			       attributes} {
    #----------------------------------------------------------
    #   marshal filter and attributes parameter
    #----------------------------------------------------------
    regsub -all {\(} $filterString " \{" filterString
    regsub -all {\)} $filterString "\} " filterString

    set berFilter [buildUpFilter $filterString]

    set berAttributes ""
    foreach attribute $attributes {
        append berAttributes [asnOctetString $attribute]
    }

    #----------------------------------------------------------
    #   marshal search request packet and send it
    #----------------------------------------------------------
    set request [asnApplicationConstr 3             \
                        [asnOctetString $baseObject]    \
                        [asnEnumeration $scope]         \
                        [asnEnumeration $derefAliases]  \
                        [asnInteger     $sizeLimit]     \
                        [asnInteger     $timeLimit]     \
                        [asnBoolean     $attrsOnly]     \
                        $berFilter                      \
                        [asnSequence    $berAttributes] \
                ]                                   
                
}
#-----------------------------------------------------------------------------
#    searchNext - returns the next result of an LDAP search
#
#-----------------------------------------------------------------------------
proc ldap::searchNext { handle } {
    CheckHandle $handle

    upvar #0 $handle conn

    if {! [::info exists conn(searchInProgress)]} then {
	return -code error \
	    "No search in progress"
    }

    set result {}
    set lastPacket 0

    #----------------------------------------------------------
    #   Wait for a search response packet
    #----------------------------------------------------------

    set response [WaitForResponse $handle $conn(searchInProgress)]
    debugData searchResponse $response

    asnGetApplication response appNum

    if {$appNum == 4} {
        trace "Search Response Continue"
	#----------------------------------------------------------
	#   unmarshal search data packet
	#----------------------------------------------------------
	asnGetOctetString response objectName
	asnGetSequence    response attributes
	set result_attributes {}
	while { [string length $attributes] != 0 } {
	    asnGetSequence attributes attribute
	    asnGetOctetString attribute attrType
	    asnGetSet  attribute attrValues
	    set result_attrValues {}
	    while { [string length $attrValues] != 0 } {
		asnGetOctetString attrValues attrValue
		lappend result_attrValues $attrValue
	    }
	    lappend result_attributes $attrType $result_attrValues
	}
	set result [list $objectName $result_attributes]
    } elseif {$appNum == 5} {
        trace "Search Response Done"
	#----------------------------------------------------------
	#   unmarshal search final response packet
	#----------------------------------------------------------
	asnGetEnumeration response resultCode
	asnGetOctetString response matchedDN
	asnGetOctetString response errorMessage
	set result {}
	FinalizeMessage $handle $conn(searchInProgress)
        unset conn(searchInProgress) 
        
	if {$resultCode != 0} {
        return -code error \
		-errorcode [list LDAP [resultCode2String $resultCode] $matchedDN $errorMessage] \
		"LDAP error [resultCode2String $resultCode] : $errorMessage"
	}
    } else {
	 error "unexpected application number ($appNum != 4 or 5)"
    }

    return $result
}

#-----------------------------------------------------------------------------
#    searchEnd - end an LDAP search
#
#-----------------------------------------------------------------------------
proc ldap::searchEnd { handle } {
    CheckHandle $handle

    upvar #0 $handle conn

    if {! [::info exists conn(searchInProgress)]} then {
        # no harm done, just do nothing
	return 
    }
    abandon $handle $conn(searchInProgress)	
    FinalizeMessage $handle $conn(searchInProgress)
    
    unset conn(searchInProgress)
    return
}

#-----------------------------------------------------------------------------
# 
#    Send an LDAP abandon message 
#
#-----------------------------------------------------------------------------    
proc ldap::abandon {handle messageId} {
    CheckHandle $handle

    upvar #0 $handle conn
    trace "MessagesPending: [string length $conn(messageId)]"
    set request [asnApplication 16      	\
                        [asnInteger $messageId]         \
                ]                                  	
    SendMessageNoReply $handle $request                
}

#-----------------------------------------------------------------------------
#    modify  -  provides attribute modifications on one single object (DN):
#                 o replace attributes with new values
#                 o delete attributes (having certain values)
#                 o add attributes with new values
#
#-----------------------------------------------------------------------------
proc ldap::modify { handle dn
                    attrValToReplace { attrToDelete {} } { attrValToAdd {} } } {

    CheckHandle $handle

    upvar #0 $handle conn

    set lrep {}
    foreach {attr value} $attrValToReplace {
	lappend lrep $attr [list $value]
    }

    set ldel {}
    foreach {attr value} $attrToDelete {
	if {[string equal $value ""]} then {
	    lappend ldel $attr {}
	} else {
	    lappend ldel $attr [list $value]
	}
    }

    set ladd {}
    foreach {attr value} $attrValToAdd {
	lappend ladd $attr [list $value]
    }

    modifyMulti $handle $dn $lrep $ldel $ladd
}


#-----------------------------------------------------------------------------
#    modify  -  provides attribute modifications on one single object (DN):
#                 o replace attributes with new values
#                 o delete attributes (having certain values)
#                 o add attributes with new values
#
#-----------------------------------------------------------------------------
proc ldap::modifyMulti {handle dn
                    attrValToReplace {attrValToDelete {}} {attrValToAdd {}}} {

    CheckHandle $handle
    upvar #0 $handle conn

    set operationAdd     0
    set operationDelete  1
    set operationReplace 2

    set modifications ""

    #------------------------------------------------------------------
    #   marshal attribute modify operations
    #    - always mode 'replace' ! see rfc2251:
    #
    #        replace: replace all existing values of the given attribute
    #        with the new values listed, creating the attribute if it
    #        did not already exist.  A replace with no value will delete
    #        the entire attribute if it exists, and is ignored if the
    #        attribute does not exist.
    #
    #------------------------------------------------------------------
    append modifications [ldap::packOpAttrVal $operationReplace \
				$attrValToReplace]

    #------------------------------------------------------------------
    #   marshal attribute add operations
    #
    #------------------------------------------------------------------
    append modifications [ldap::packOpAttrVal $operationAdd \
				$attrValToAdd]

    #------------------------------------------------------------------
    #   marshal attribute delete operations
    #
    #     - a non-empty value will trigger to delete only those
    #       attributes which have the same value as the given one
    #
    #     - an empty value will trigger to delete the attribute
    #       in all cases
    #
    #------------------------------------------------------------------
    append modifications [ldap::packOpAttrVal $operationDelete \
				$attrValToDelete]

    #----------------------------------------------------------
    #   marshal 'modify' request packet and send it
    #----------------------------------------------------------
    set request [asnApplicationConstr 6              \
                        [asnOctetString $dn ]            \
                        [asnSequence    $modifications ] \
                ]                                    
    set messageId [SendMessage $handle $request]            
    debugData modifyRequest $request
    set response [WaitForResponse $handle $messageId]
    FinalizeMessage $handle $messageId    
    debugData bindResponse $response

    asnGetApplication response appNum
    if { $appNum != 7 } {
         error "unexpected application number ($appNum != 7)"
    }
    asnGetEnumeration response resultCode
    asnGetOctetString response matchedDN
    asnGetOctetString response errorMessage
    if {$resultCode != 0} {
        return -code error \
		-errorcode [list LDAP [resultCode2String $resultCode] $matchedDN $errorMessage] \
		"LDAP error [resultCode2String $resultCode] '$matchedDN': $errorMessage"    
    }
}

proc ldap::packOpAttrVal {op attrValueTuples} {
    set p ""
    foreach {attrName attrValues} $attrValueTuples {
	set l {}
	foreach v $attrValues {
	    lappend l [asnOctetString $v]
	}
        append p [asnSequence                        \
		    [asnEnumeration $op ]            \
		    [asnSequence                     \
			[asnOctetString $attrName  ] \
			[asnSetFromList $l]          \
		    ]                                \
		]
    }
    return $p
}


#-----------------------------------------------------------------------------
#    add  -  will create a new object using given DN and sets the given
#            attributes. Multiple value attributes may be used, provided
#            that each attr-val pair be listed.
#
#-----------------------------------------------------------------------------
proc ldap::add { handle dn attrValueTuples } {

    CheckHandle $handle

    #
    # In order to handle multi-valuated attributes (see bug 1191326 on
    # sourceforge), we walk through tuples to collect all values for
    # an attribute.
    # http://sourceforge.net/tracker/index.php?func=detail&atid=112883&group_id=12883&aid=1191326
    #

    foreach { attrName attrValue } $attrValueTuples {
	lappend avpairs($attrName) $attrValue
    }

    return [addMulti $handle $dn [array get avpairs]]
}

#-----------------------------------------------------------------------------
#    addMulti -  will create a new object using given DN and sets the given
#                attributes. Argument is a list of attr-listOfVals pair.
#
#-----------------------------------------------------------------------------
proc ldap::addMulti { handle dn attrValueTuples } {

    CheckHandle $handle

    upvar #0 $handle conn

    #------------------------------------------------------------------
    #   marshal attribute list
    #
    #------------------------------------------------------------------
    set attrList ""

    foreach { attrName attrValues } $attrValueTuples {
	set valList {}
	foreach val $attrValues {
	    lappend valList [asnOctetString $val]
	}
	append attrList [asnSequence                         \
			    [asnOctetString $attrName ]      \
			    [asnSetFromList $valList]        \
			]
    }

    #----------------------------------------------------------
    #   marshal search 'add' request packet and send it
    #----------------------------------------------------------
    set request [asnApplicationConstr 8             \
                        [asnOctetString $dn       ] \
                        [asnSequence    $attrList ] \
                ]                               
                
    set messageId [SendMessage $handle $request]
    debugData addRequest $request
    set response [WaitForResponse $handle $messageId]
    FinalizeMessage $handle $messageId    
    debugData bindResponse $response

    asnGetApplication response appNum
    if { $appNum != 9 } {
         error "unexpected application number ($appNum != 9)"
    }
    asnGetEnumeration response resultCode
    asnGetOctetString response matchedDN
    asnGetOctetString response errorMessage
    if {$resultCode != 0} {
        return -code error \
		-errorcode [list LDAP [resultCode2String $resultCode] $matchedDN $errorMessage] \
		"LDAP error [resultCode2String $resultCode] '$matchedDN': $errorMessage"
    }
}

#-----------------------------------------------------------------------------
#    delete  -  removes the whole object (DN) inclusive all attributes
#
#-----------------------------------------------------------------------------
proc ldap::delete { handle dn } {

    CheckHandle $handle

    upvar #0 $handle conn

    #----------------------------------------------------------
    #   marshal 'delete' request packet and send it
    #----------------------------------------------------------
    set request [asnApplication 10 $dn ] 
    set messageId [SendMessage $handle $request]
    debugData deleteRequest $request
    set response [WaitForResponse $handle $messageId]
    FinalizeMessage $handle $messageId
        
    debugData deleteResponse $response

    asnGetApplication response appNum
    if { $appNum != 11 } {
         error "unexpected application number ($appNum != 11)"
    }
    asnGetEnumeration response resultCode
    asnGetOctetString response matchedDN
    asnGetOctetString response errorMessage
    if {$resultCode != 0} {
        return -code error \
		-errorcode [list LDAP [resultCode2String $resultCode] $matchedDN $errorMessage] \
		"LDAP error [resultCode2String $resultCode] '$matchedDN': $errorMessage"
    }
}


#-----------------------------------------------------------------------------
#    modifyDN  -  moves an object (DN) to another (relative) place
#
#-----------------------------------------------------------------------------
proc ldap::modifyDN { handle dn newrdn { deleteOld 1 } {newSuperior ! } } {

    CheckHandle $handle

    upvar #0 $handle conn

    #----------------------------------------------------------
    #   marshal 'modifyDN' request packet and send it
    #----------------------------------------------------------

    if {[string equal $newSuperior "!"]} then {
        set request [asnApplicationConstr 12                 \
			    [asnOctetString $dn ]            \
			    [asnOctetString $newrdn ]        \
			    [asnBoolean     $deleteOld ]     \
		    ]                                         
		    
    } else {
	set request [asnApplicationConstr 12                 \
			    [asnOctetString $dn ]            \
			    [asnOctetString $newrdn ]        \
			    [asnBoolean     $deleteOld ]     \
			    [asnContext     0 $newSuperior]  \
		    ]                                       
    }
    set messageId [SendMessage $handle $request]
    debugData modifyRequest $request
    set response [WaitForResponse $handle $messageId]

    asnGetApplication response appNum
    if { $appNum != 13 } {
         error "unexpected application number ($appNum != 13)"
    }
    asnGetEnumeration response resultCode
    asnGetOctetString response matchedDN
    asnGetOctetString response errorMessage
    if {$resultCode != 0} {
        return -code error \
		-errorcode [list LDAP [resultCode2String $resultCode] $matchedDN $errorMessage] \
		"LDAP error [resultCode2String $resultCode] '$matchedDN': $errorMessage"

    }
}

#-----------------------------------------------------------------------------
#    disconnect
#
#-----------------------------------------------------------------------------
proc ldap::disconnect { handle } {

    CheckHandle $handle
    
    upvar #0 $handle conn

    # should we sent an 'unbind' ?
    catch {close $conn(sock)}
    unset conn

    return
}



#-----------------------------------------------------------------------------
#    trace
#
#-----------------------------------------------------------------------------
proc ldap::trace { message } {

    variable doDebug

    if {!$doDebug} return

    puts stderr $message
}


#-----------------------------------------------------------------------------
#    debugData
#
#-----------------------------------------------------------------------------
proc ldap::debugData { info data } {

    variable doDebug

    if {!$doDebug} return

    set len [string length $data]
    trace "$info ($len bytes):"
    set address ""
    set hexnums ""
    set ascii   ""
    for {set i 0} {$i < $len} {incr i} {
        set v [string index $data $i]
        binary scan $v H2 hex
        binary scan $v c  num
        set num [expr {( $num + 0x100 ) % 0x100}]
        set text .
        if {$num > 31} {
            set text $v
        }
        if { ($i % 16) == 0 } {
            if {$address != ""} {
                trace [format "%4s  %-48s  |%s|" $address $hexnums $ascii ]
                set address ""
                set hexnums ""
                set ascii   ""
            }
            append address [format "%04d" $i]
        }
        append hexnums "$hex "
        append ascii   $text
        #trace [format "%3d %2s %s" $i $hex $text]
    }
    if {$address != ""} {
        trace [format "%4s  %-48s  |%s|" $address $hexnums $ascii ]
    }
    trace ""
}
