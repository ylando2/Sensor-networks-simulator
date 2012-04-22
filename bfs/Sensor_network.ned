//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003 Ahmet Sekercioglu
// Copyright (C) 2003-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc1
    gates:
        in: Tin;
        out: Tout;
        in: in[];
        out: out[];
endsimple

module sensorEmu
    gates:
        in: in[];
        out: out[];
    submodules:
        Txc1: Txc1;
            display: "p=240,160;i=old/proc1";
    connections:
        for i=0..sizeof(in)-1 do
            in[i] --> Txc1.in++;
        endfor;
        for i=0..sizeof(out)-1 do
            out[i] <-- Txc1.out++;
        endfor;
        Txc1.Tout --> Txc1.Tin;
endmodule

module Sensor_network

    submodules:
        Node1: sensorEmu;
            display: "p=327,64;i=block/broadcast";
        Node4: sensorEmu;
            display: "i=block/broadcast;p=67,224";
        Node5: sensorEmu;
            display: "p=120,64;i=block/broadcast";
        Node2: sensorEmu;
            display: "p=360,224;i=block/broadcast";
        Node3: sensorEmu;
            display: "p=200,312;i=block/broadcast";
    connections:
        Node1.out++ --> slow --> Node4.in++;
        Node4.out++ --> slow --> Node5.in++;
        Node5.out++ --> slow --> Node1.in++;
        Node5.out++ --> slow --> Node4.in++;
        Node4.out++ --> slow --> Node1.in++;
        Node1.out++ --> slow --> Node5.in++;
        Node4.out++ --> slow --> Node3.in++;
        Node3.out++ --> slow --> Node4.in++;
        Node3.out++ --> slow --> Node2.in++;
        Node2.out++ --> slow --> Node3.in++;
        Node2.out++ --> slow --> Node1.in++;
        Node1.out++ --> slow --> Node2.in++;
    display: "b=438,376;o=#8be9b6";
endmodule

network sensor_network : Sensor_network
endnetwork
channel slow
    delay 50ms;
endchannel

