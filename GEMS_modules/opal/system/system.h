
/*
    Copyright (C) 1999-2005 by Mark D. Hill and David A. Wood for the
    Wisconsin Multifacet Project.  Contact: gems@cs.wisc.edu
    http://www.cs.wisc.edu/gems/

    --------------------------------------------------------------------

    This file is part of the Opal Timing-First Microarchitectural
    Simulator, a component of the Multifacet GEMS (General 
    Execution-driven Multiprocessor Simulator) software toolset 
    originally developed at the University of Wisconsin-Madison.

    Opal was originally developed by Carl Mauer based upon code by
    Craig Zilles.

    Substantial further development of Multifacet GEMS at the
    University of Wisconsin was performed by Alaa Alameldeen, Brad
    Beckmann, Jayaram Bobba, Ross Dickson, Dan Gibson, Pacia Harper,
    Milo Martin, Michael Marty, Carl Mauer, Michelle Moravan,
    Kevin Moore, Manoj Plakal, Daniel Sorin, Min Xu, and Luke Yen.

    --------------------------------------------------------------------

    If your use of this software contributes to a published paper, we
    request that you (1) cite our summary paper that appears on our
    website (http://www.cs.wisc.edu/gems/) and (2) e-mail a citation
    for your published paper to gems@cs.wisc.edu.

    If you redistribute derivatives of this software, we request that
    you notify us and either (1) ask people to register with us at our
    website (http://www.cs.wisc.edu/gems/) or (2) collect registration
    information and periodically send it to us.

    --------------------------------------------------------------------

    Multifacet GEMS is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    Multifacet GEMS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Multifacet GEMS; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307, USA

    The GNU General Public License is contained in the file LICENSE.

### END HEADER ###
*/
#ifndef __SYSTEM_H
#define __SYSTEM_H

/*------------------------------------------------------------------------*/
/* Includes                                                               */
/*------------------------------------------------------------------------*/
#include "diagnosis.h"


/// action function to call when breakpoint occurs
typedef void (system_t::*breakpointf_t)( uint64, access_t, void * );

/// hash entry for table of breakpoints set in the system
typedef struct breakpoint_action {
  la_t           address;
  breakpointf_t  action;
  void          *user_data;
} breakpoint_action_t;

/// table of breakpoints
typedef map<breakpoint_id_t, breakpoint_action_t *> BreakpointTable;

/*------------------------------------------------------------------------*/
/* Class declaration(s)                                                   */
/*------------------------------------------------------------------------*/

/**
* This object contains the entire out-of-order processor simulator.
*
* It contains a collection of processors (and their associated state)
* The static variable "inst" points to the single instance of the
* system in the program.
*
* @see     pstate_t
* @author  cmauer
* @version $Id: system.h 1.52 06/02/13 18:49:23-06:00 mikem@maya.cs.wisc.edu $
*/
class system_t {

public:
  /** @name Constructor(s) / Destructor */
  //@{
  /**
   * Constructor: Calls SimIcs API to initialize the system
   */
  system_t( const char *configurationFile );

  /**
   * Destructor: frees object.
   */
  ~system_t( void );
  //@}

  /** @name System wide test code
   */
  //@{

  /** Run the "main-loop" of the out-of-order simulator.
   *  @param cycleCount The number of cycles to simulate in the processor
   */
  void        simulate( uint64 instrCount );  
  /** detected error, need to rollback */
  void        detectedSymp( void ) ;
  /** halt the main loop of simulation as soon as possible */
  void        breakSimulation( void );
  /** print out the statistics related to the build parameters */
  void        printBuildParameters( void );
  /** print statistics associated with each instruction */
  void        printStats( void );
  /** print the in flight instructions */
  void        printInflight( void );
  /** print memory allocation stats */
  void        printMemoryStats( void );
  /** gets the global cycle in simulation time */
  tick_t      getGlobalCycle( void );
  /** step processors using inorder functional simulation. */
  void        stepInorder( uint64 instrCount );  
  
  /** breakpoint methods */
  /** post a breakpoint to the system */
  void        postedBreakpoint( uint32 id, uint32 access );
  /** post a 'magic-call' (end-transaction) event to the system */
  void        postedMagicBreak( void );
  /** dereference operator using simics's physical memory */
  uint64      simicsGetThread( uint32 *pid );
  /** Add an breakpoint to simics memory: your function will be called
   *  back when this breakpoint is reached. */
  void        setSimicsBreakpoint( la_t address, void *user_data );
  /** a more full featured breakpoint interface */
  void        setSimicsBreakpoint( la_t address, breakpoint_kind_t type,
                                   access_t access, breakpointf_t action_f,
                                   void *user_data );
  /** basic breakpoint: prints out the address and the "name" */
  void        breakpoint_print( uint64 address, access_t access, void *name );
  /** os breakpoint: called when the OS schedules a different thread */
  void        breakpoint_os_resume( uint64 address, access_t access, void *name );
  void        breakpoint_memcpy( uint64 address, access_t access, void *unused );

  /** thread switch breakpoint */
  void        breakpoint_switch( uint64 address, access_t access, void *data );
  /** set up breakpoints for this sequencer */
  void        setupBreakpointDispatch( bool symbolTracing );

  /// opens a log file group
  void        openLogfiles( char *logname );
  /// opens a log file group
  void        openFaultLog( char *logname );
  /// Sets the fault type
  void		  setFaultType( int t ) ;
  /// Sets the fault type
  void		  setFaultBit( int b ) ;
  /// Sets the fault type
  void		  setFaultStuckat( int s ) ;
  /// Sets the faulty entry
  void		  setFaultyReg( int r ) ;
  /// Sets the faulty entry src or dest location
  void		  setSrcDestReg( int sd ) ;
  void		  setStartLogging( int sd ) ;
  void		  setComparePoint( int sd ) ;


// Diagnosis functions
  void takeCheckpoint(int core_id);
  void prepareRollback(int core_id);
  void postRollback(int core_id);

  llb_t* getLLB(int core_id);
  func_inst_buffer_t& getFuncLog(int core_id);
  void putLLB(llb_t* load_buf, int core_id);
  void putFuncLog(func_inst_buffer_t func_inst_buf, int core_id);
  void disableInterrupts(int core_id);
  void clearLLB(int);
  bool isLLBFull(int core_id);
  bool compareState(core_state_t *ps, int core_id); 

  uint64 getLocalCycle(int core_id);

  bool getArchMismatch(int core_id);

  //----------------------------------------------- GATE LEVEL SAFSIM ----------------------------------------------//
  /// Sets the fault gateId type
  void		  setFaultyGateId( int s ) ;
  /// Sets the faulty fanoutId
  void		  setFaultyFanoutId( int s ) ;
  /// Print the fault stats
//  void		  printSafFaultStats( ) ;
  void        setChkptInterval(int interval) ;
  void		  startCheckpointing() ;
  void		  setServerClient( int mode ) ;
  void		  connectAmber() ;

  // NCVLOG RELATED

  /// Sets the faulty machine (faulty net) name
  void setFaultyMachine( string s );
  void setStimPipe( string s );
  void setResponsePipe( string s );
  /// Print saf statistics - vlog
  // void printVlogStats( );
  //----------------------------------------------- GATE LEVEL SAFSIM ----------------------------------------------//

  /// Sets the faulty core
  void		  setFaultyCore( int c ) ;
  /// Print the fault stats
  void		  printFaultStats( ) ;
  /// prints the log file header
  void        printLogHeader( void );
  /// closes a log file group
  void        closeLogfiles( void );
  /// closes a log file group
  void        closeFaultLog( void );
  // set start cycle for inf loop detector
  void        setInfLoopCycle(uint64 cycle);

  void        setFaultInjInst(int value);

  /** @name system status & control code
   */
  //@{
  /// Enumeration of the system modes: OK, BREAK (and other modes as well)
  enum sim_status_t {
    SIMSTATUS_OK,
    SIMSTATUS_TRACING,
    SIMSTATUS_WARMUP,
    SIMSTATUS_BREAK,
    SIMSTATUS_MLP_TRACE,
    SIMSTATUS_SYMBOL_MODE
  };
  /// stops simulation (at the next stopping point)
  void        haltSimulation( void );
  /// exits simulation immediately
  static void systemExit( void );

  /// queries status of simulation.
  sim_status_t getSimulationStatus( void ) {
    return m_sim_status;
  }
  /// queries status of warmingup
  bool        isSimulating( void ) {
    return (m_sim_status == SIMSTATUS_OK);
  }
  /// queries status of warmingup
  bool        isWarmingUp( void ) {
    return (m_sim_status == SIMSTATUS_WARMUP);
  }
  /// queries status of tracing.
  bool        isTracing( void ) {
    return (m_sim_status == SIMSTATUS_TRACING);
  }
  /// queries if ruby is loaded
  bool        isRubyLoaded( void ) {
    return (m_ruby_api != NULL);
  }
  /// queries if amber is loaded
  bool        isAmberLoaded( void ) {
    return (m_amber_api != NULL);
  }

  bool isUnknownRun() { 
	  return m_is_unknown_run;
  }

  void setDetected() { m_is_detected = true ; }
  bool isDetected() { return m_is_detected ; }

  //@}

  /** @name New tracing interfaces */
  //@{
  /// processes a system command
  set_error_t commandSetDispatch( attr_value_t *val );
  //@}
  
  /** @name Tracing interfaces */
  //@{
  /// opens trace files
  void        openTrace( char *tracename );
  /// steps log files
  void        writeTraceStep( void );
  /// steps by a factor of two
  void        writeSkipTraceStep( void );
  /// closes trace files
  void        closeTrace( void );

  /// opens a branch trace files
  void        openBranchTrace( char *tracename );
  /// writes a branch trace step
  void        writeBranchStep( void );
  /// closes the current file, and opens the next sequential file
  bool        writeBranchNextFile( void );
  /// closes trace files
  void        closeBranchTrace( void );

  /// attach a trace file to each processor state
  void        attachTrace( char *baseFilename, char *traceFilename );
  /// attach a memory trace file for each processor
  void        attachMemTrace( char *baseFilename, char *traceFilename );
  /// attach a TLB trace file for each processor
  void        attachTLBTrace( char *baseFilename, char *traceFilename );

  /// debugging: validate a trace file by printing its contents out
  void        printTrace( void );
  //@}

  /// read a trace, and save the instruction mapping
  void        saveInstructionMap( char *baseFilename, char *filename );
  /// attach an instruction map to a processor state
  void        readInstructionMap( char *baseFilename, char *traceFilename,
                                  bool validate );

  /// install hap handler
  void        installHapHandlers( void );
  /// remove hap handler
  void        removeHapHandlers( void );

  /** SIMICS specific: install memory timing interface */
  void        installMemoryHierarchy( sim_status_t status );
  /** SIMICS specific: remove memory timing interface */
  void        removeMemoryHierarchy( void );

  /** SIMICS specific: install memory 'snoop' interface */
  void        installMemoryObserver( void );
  /** SIMICS specific: remove memory 'snoop' interface */
  void        removeMemoryObserver( void );

  /** SIMICS specific: install a exception handler */
  void        installExceptionHandler( sim_status_t status );
  /** SIMICS specific: removes the exception handler */
  void        removeExceptionHandler( void );

  /** RUBY specific: query ruby for its API interface.
   */
  void        queryRubyInterface( void );
  /** RUBY specific: ruby is unloaded, so remove its API interface */
  void        removeRubyInterface( void );
  
  /** RUBY specific:
   *  opal is loaded, notify ruby and get api's if possible
   */
  static void rubyNotifySend( int status );

  /** RUBY specific:
   *  recieve a notification from ruby
   */
  static void rubyNotifyRecieve( int status );

  /** RUBY specific:
   *  query a processor for the number of instructions retired
   */
  static integer_t rubyInstructionQuery( int cpuNumber );

  /** RUBY specific:
   *   completedRequest is called when a data is available for the processor
   *   from a previous makeRequest() call. Opal will maintain the invariant
   *   that only one request per cache line is issued to ruby. The cpu number,
   *   and the physical block address will completely specify the request.
   */
  static void rubyCompletedRequest( int cpuNumber, pa_t physicalAddr );


  /// Amber specific function
  static void amberNextChkpt() ;
  
  /// write configuration info to disk
  void        writeConfiguration( char *configurationName );
  /// read configuration info from disk
  void        readConfiguration( char *configurationName );

  /// write parameters to a file

  /// read parameters from a file
  void        readParameters( char *parameterFile );
  //@}

  /// The name of the configuration file
  const char   *m_configFile;

  /// The number of processors in the system
  int           m_numProcs;

  multicore_diagnosis_t* m_multicore_diagnosis;

  /// An array of pointers to the sequencers in the system
  pseq_t      **m_seq;
  
  /// An array of pointers to the simics state interfaces in the system
  pstate_t    **m_state;

  /// A tracer in the system
  ptrace_t     *m_trace;

  /** A system-wide memory alias table: tracks the last writer to each line.
   *     A map of physical address (cache line size) -> flow instruction.
   *     Either this or the local (per processor) table is used, depending
   *     on the CHAIN_MP_MODE parameter.
   */
  CFGIndex      m_alias_table;
  
  /// The "memory-dependence-chain" analyzer: present in most modern systems :)
  chain_t     **m_chain;

  /// The "symbolic" information tracker (reverse look up on process & PA)
  symtrace_t   *m_symtrace;

  /// The static system instance
  static system_t *inst;

  /// A statistics structure for tracking system wide stats
  sys_stat_t   *m_sys_stat;

  /// Table of breakpoints installed on simics's memory locations
  BreakpointTable *m_breakpoint_table;

  /// API for 'ruby' memory hierarchy to call opal
  mf_opal_api_t *m_opal_api;
  /// API for opal to call ruby
  mf_ruby_api_t *m_ruby_api;
  /// API for opal to call amber
  amber_api_t *m_amber_api;

  void newAmberChkpt() {
	  if(isAmberLoaded()) {
		  (*m_amber_api->newChkpt)() ;
	  }
  }
  
  void initAmber() {
	  if(isAmberLoaded()) {
		  DEBUG_OUT("Sending initAmber()\n") ;
		  (*m_amber_api->initialize)() ;
	  }
  }

  void notifyAmberDrainBuffer() {
	  if(isAmberLoaded()) {
		  (*m_amber_api->drainBuffer)() ;
	  }
  }
  
  void notifyAmberRecovery() {
	  if(isAmberLoaded()) {
		  (*m_amber_api->recover)() ;
	  }
  }
  
  /// The memory hierarchy memory interface
  timing_model_interface_t  m_timing_interface;

  /// boolean var indicating if the memory hierarchy is installed
  bool          m_mhinstalled;

  /// true indicates the snoop device is installed (observes I/O actions)
  bool          m_snoop_installed;

  /// simulation message buffer: contains warning and error messages
  char          m_sim_message_buffer[256];

#ifdef MEASURE_FP
  int getFP(int i, int j);
#endif

#ifdef LL_DECODER
  dyn_execute_type_t *opcode_to_exec_type;
  void initOpcodeExecTable();
  dyn_execute_type_t lookupOpExecTab(int x);
#endif

  int server_client_mode;

private:
  /// 'hap' handle for magic-call instructions (to count transactions)
  hap_handle_t  m_magic_call_hap;

  /// 'hap' handle for exception events (interrupts, etc)
  hap_handle_t  m_exception_haphandle;

  /// The global cycle count (global time)
  tick_t        m_global_cycles;

  /// status of simulation
  sim_status_t  m_sim_status;

  bool m_is_unknown_run;
  bool m_is_detected ;
  int CORE_ID;

  int server_start_num;
  int client_start_num;

  /** initalize the thread pointers of each sequencers. done once on init. */
  void          initThreadPointers( void );

	uint64 minCommittedInstr();
	bool continueSimulation( uint64 );

};

/*------------------------------------------------------------------------*/
/* Global functions                                                       */
/*------------------------------------------------------------------------*/

/// macro that stops simulation at the next convenient point: use for errors
#define HALT_SIMULATION \
   system_t::inst->haltSimulation();

/// macro that exits immediately: use for fatal (non-assertion) errors
#define SYSTEM_EXIT \
   system_t::systemExit();

/**
 * MMU hap handler for dealing with context switches.
 * @param system_obj  same as system::inst
 * @param context     The (possibly new) context which was written
 * @return 0 if no problem -1 to stop simics
 */
extern "C" void system_mmu_hap_handler( void *system_obj, uint64 context );

/**
 * Memory 'snoop' interface.
 */
extern "C" cycles_t system_memory_snoop( conf_object_t *obj,
                                         conf_object_t *space,
                                         map_list_t *map,
                                         memory_transaction_t *mem_op);

/**
 * Memory hierarchy timing interface.
 */
extern "C" cycles_t system_memory_operation( conf_object_t *obj,
                                             conf_object_t *space,
                                             map_list_t *map,
                                             memory_transaction_t *mem_op);

#endif /* __SYSTEM_H */
