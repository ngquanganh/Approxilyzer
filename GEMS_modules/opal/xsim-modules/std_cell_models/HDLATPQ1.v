// 
// Copyright (C) 2003 Virtual Silicon Technology Inc. All Rights Reserved.
// Silicon Ready, The Heart of Great Silicon, and the Virtual Silicon
// logo are registered trademarks of Virtual Silicon Technology Inc.  All
// other trademarks are the property of their respective owner.
// 
// Virtual Silicon Technology Inc.
// 1322 Orleans Drive
// Sunnyvale, CA 94089-1116
// Phone : (408) 548-2700
// Fax   : (408) 548-2750
// Web Site : http://www.virtual-silicon.com
// 
// File Name:       HDLATPQ1.v
// Library Name:    umce13h210t3
// Library Release: 1.2
// Product:         High Density Standard Cells
// Process:         UMC L130E-HS-FSG
// Generated:       07/16/2003 15:28:23
// ------------------------------------------------------------------------
//  
// $RCSfile: HDLATPQ1.v,v $ 
// $Source: /syncdisk/rcs/common/verilog/5.1.4.3/sc/RCS/HDLATPQ1.v,v $ 
// $Date: 2003/04/23 22:39:23 $ 
// $Revision: 1.2 $ 
//  
// ---------------------- 
// Verilog dump Timing Insertion Version 1.5

// Verilog dump veralc Version 1.9
/*****************************************************************************/
/*                                                                           */
/*  CellRater, version 5.1.4.3 production                                    */
/*  Created:  Thu Feb 20 16:36:22 2003 by sasana                             */
/*    for Verilog Simulator:  verilog-xl                                     */
/*                                                                           */
/*****************************************************************************/
`timescale 1 ns / 1 ps
`define VCC 1'b1
`define VSS 0
`celldefine
`suppress_faults
`enable_portfaults
module HDLATPQ1(D, GB, Q);
input D;
input GB;
output Q;
reg notifier;
wire SMC_EN_IN;
not SMC_I0(SMC_EN_IN, GB);
`ifdef no_tchk
`else
not SMC_I1(SMC_IQN, SMC_IQ);
`endif
ldlatch_p0 SMC_I2(.q(SMC_IQ), .d(D), .en(SMC_EN_IN), .clear(`VCC), .preset(`VCC), .notifier(notifier));
//  output pins
buf SMC_I3(Q, SMC_IQ);
specify
// arc D --> Q
  ( D => Q ) = (1,1);
// arc GB --> Q
  ( GB => Q ) = (1,1);
`ifdef no_tchk
`else
  $setup( negedge D,  posedge GB, 1, notifier);
  $setup( posedge D,  posedge GB, 1, notifier);
  $hold( posedge GB,  posedge D, 1, notifier);
  $hold( posedge GB,  negedge D, 1, notifier);
  $width( negedge GB, 1, 0, notifier);
  $width( posedge GB, 1, 0, notifier);
  $period( posedge GB, 0, notifier);
  $period( negedge GB, 0, notifier);
`endif
endspecify
endmodule // HDLATPQ1 //
`disable_portfaults
`nosuppress_faults
`endcelldefine
