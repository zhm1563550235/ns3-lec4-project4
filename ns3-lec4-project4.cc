/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

// Default Network Topology
//
//   Wifi_2 10.1.3.0
//                         AP
// *   *    *     *    *    *
// |   |    |     |    |    |    10.1.1.0
// n8  n9   n10   n11  n12  n0 -------------- n1   n2   n3   n4   n5   n6   n7
//                           point-to-point  |    |    |    |    |    |    |
//                                           *    *    *    *    *    *    *
//                                          AP  wifi_1 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ns3-lec4-project4");  //定义记录组件 


int main (int argc, char *argv[])
{
  bool verbose = true; //记录组件boot or not
  uint32_t nWifi1 = 6; //wifi1节点数量
  uint32_t nWifi2 = 6; //wifi2节点数量
  bool tracing = false;

  LogComponentEnable("ns3-lec4-project4",LOG_LEVEL_ALL);

  CommandLine cmd;
  cmd.AddValue ("nWifi1", "Number of wifi STA nodes/devices", nWifi1);
  cmd.AddValue ("nWifi2", "Number of wifi STA devices", nWifi2);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing","Enable pcap tracing",tracing);

  cmd.Parse (argc,argv);

  if (nWifi1 > 250 || nWifi2>250)
    {
      std::cout << "Number of wifi nodes specified exceeds the mobility bounding box" << std::endl;
      exit (1);
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }



  
  /*wifi_1 config*/
  NodeContainer wifi1StaNodes;
  wifi1StaNodes.Create (nWifi1);
  NodeContainer wifi1ApNode;
  wifi1ApNode.Create(1);

  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
  channel1.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default ();
  phy1.SetChannel (channel1.Create ());

  WifiHelper wifi1 = WifiHelper::Default ();
  wifi1.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac1= NqosWifiMacHelper::Default ();

  Ssid ssid1 = Ssid ("wifi_1");
  mac1.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid1),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer sta1Devices;    //第一个wifi网络的sta节点设备
  sta1Devices = wifi1.Install (phy1, mac1, wifi1StaNodes);

  mac1.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid1));

  NetDeviceContainer ap1Devices;
  ap1Devices = wifi1.Install (phy1, mac1, wifi1ApNode);

  MobilityHelper mobility1;
  
  /*mobility1.SetPositionAllocator("ns3::GridPositionAllocator",
                                 "MinX",DoubleValue(0.0),
                                 "MinY",DoubleValue(0.0),
                                 "DeltaX",DoubleValue(5.0),
                                 "DeltaY",DoubleValue(10.0),
                                 "GridWidth",UintegerValue(3), 
                                 "LayoutType",StringValue("RowFirst")); */

   Ptr<ListPositionAllocator>positionAlloc1=CreateObject<ListPositionAllocator>();
   positionAlloc1->Add (Vector(5.0,0.0,0.0));
   positionAlloc1->Add (Vector(0.0,8.66,0.0));
   positionAlloc1->Add (Vector(5.0,17.32,0.0));
   positionAlloc1->Add (Vector(15.0,17.32,0.0));
   positionAlloc1->Add (Vector(20.0,8.66,0.0));
   positionAlloc1->Add (Vector(15.0,0.0,0.0));
   mobility1.SetPositionAllocator(positionAlloc1);
              
   //以随机游走的方式移动                                                     
  mobility1.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds",RectangleValue(Rectangle(-25,25,-50,50)));
  mobility1.Install (wifi1StaNodes);


  /*wifi2 config*/
  NodeContainer wifi2StaNodes;
  wifi2StaNodes.Create (nWifi2);
  NodeContainer wifi2ApNode;
  wifi2ApNode.Create(1);

  //创建无线设备于无线节点之间的互联通道，并将通道对象与物理层对象关联
  //确保所有物理层对象使用相同的底层信道，即无线信道
  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  channel2.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy2.SetChannel (channel2.Create ());

  WifiHelper wifi2 = WifiHelper::Default ();
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac2= NqosWifiMacHelper::Default ();

  Ssid ssid2 = Ssid ("wifi_2");
  mac2.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid2),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer sta2Devices;    //第二个wifi网络的sta节点设备
  sta2Devices = wifi2.Install (phy2, mac2, wifi2StaNodes);

  mac2.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid2));

  NetDeviceContainer ap2Devices;
  ap2Devices = wifi2.Install (phy2, mac2, wifi2ApNode);


  MobilityHelper mobility2;

  /*mobility2.SetPositionAllocator("ns3::GridPositionAllocator",
                                 "MinX",DoubleValue(20.0),
                                 "MinY",DoubleValue(0.0),
                                 "DeltaX",DoubleValue(5.0),
                                 "DeltaY",DoubleValue(10.0),
                                 "GridWidth",UintegerValue(3), 
                                 "LayoutType",StringValue("RowFirst"));  */                
 

  Ptr<ListPositionAllocator>positionAlloc2=CreateObject<ListPositionAllocator>();
   positionAlloc2->Add (Vector(55.0,0.0,0.0));
   positionAlloc2->Add (Vector(50.0,8.66,0.0));
   positionAlloc2->Add (Vector(55.0,17.32,0.0));
   positionAlloc2->Add (Vector(65.0,17.32,0.0));
   positionAlloc2->Add (Vector(70.0,8.66,0.0));
   positionAlloc2->Add (Vector(65.0,0.0,0.0));
   mobility2.SetPositionAllocator(positionAlloc2);  
   //以随机游走的方式移动                                                     
  mobility2.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds",RectangleValue(Rectangle(25,95,-50,50)));
  mobility2.Install (wifi2StaNodes);


 /*wifi_ap config*/
  YansWifiChannelHelper channel3 = YansWifiChannelHelper::Default ();
  channel3.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  YansWifiPhyHelper phy3 = YansWifiPhyHelper::Default ();
  phy3.SetChannel (channel3.Create ());

  WifiHelper wifi3 = WifiHelper::Default ();
  wifi3.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac3= NqosWifiMacHelper::Default ();

  Ssid ssid3 = Ssid ("ns-3-ssid");
  mac3.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid3),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer sta3Devices;    
  sta3Devices = wifi3.Install (phy3, mac3, wifi2ApNode);
  mac3.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid3));

  NetDeviceContainer ap3Devices;
  ap3Devices = wifi3.Install (phy3, mac3, wifi1ApNode);

  MobilityHelper mobility3;
  Ptr<ListPositionAllocator>positionAlloc3=CreateObject<ListPositionAllocator>();
   positionAlloc3->Add (Vector(10.0,8.66,0.0));
   positionAlloc3->Add (Vector(60.0,8.66,0.0));
   mobility3.SetPositionAllocator(positionAlloc3); 

  mobility3.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility3.Install (wifi1ApNode);
  mobility3.Install (wifi2ApNode);

  
  //已经创建了节点，设备，信道和移动模型，接下来配置协议栈
  InternetStackHelper stack;
  stack.Install (wifi1ApNode);
  stack.Install (wifi2ApNode);
  stack.Install (wifi1StaNodes);
  stack.Install (wifi2StaNodes);
  
//分配IP地址
  Ipv4AddressHelper address;
//wifi信道
  address.SetBase ("10.1.1.0", "255.255.255.0");
  address.Assign (sta1Devices);
  address.Assign (ap1Devices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  address.Assign (sta2Devices);
  Ipv4InterfaceContainer wifi2StaInterfaces;
  wifi2StaInterfaces = address.Assign (ap2Devices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (sta3Devices);
  address.Assign (ap3Devices);
//放置echo服务端程序在最右边的csma节点,端口为9
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (wifi2StaNodes.Get(0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (30.0));
//回显客户端放在最后的STA节点，指向wifi网络的服务器，上面的节点地址，端口为9
  UdpEchoClientHelper echoClient (wifi2StaInterfaces.GetAddress(0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (512));

  ApplicationContainer clientApps1 = 
  echoClient.Install (wifi1StaNodes.Get(nWifi1-1));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));
  ApplicationContainer clientApps2 = 
  echoClient.Install (wifi1StaNodes.Get(nWifi1-2));
  clientApps2.Start (Seconds (2.5));
  clientApps2.Stop (Seconds (11.0));
  ApplicationContainer clientApps3 = 
  echoClient.Install (wifi1StaNodes.Get(nWifi1-3));
  clientApps3.Start (Seconds (3.0));
  clientApps3.Stop (Seconds (11.0));
  ApplicationContainer clientApps4 = 
  echoClient.Install (wifi1ApNode.Get(0));
  clientApps4.Start (Seconds (3.5));
  clientApps4.Stop (Seconds (12.0));

  ApplicationContainer clientApps5 = 
  echoClient.Install (wifi2StaNodes.Get(nWifi2-1));
  clientApps5.Start (Seconds (4.0));
  clientApps5.Stop (Seconds (12.0));
  ApplicationContainer clientApps6 = 
  echoClient.Install (wifi2StaNodes.Get(nWifi2-2));
  clientApps6.Start (Seconds (4.5));
  clientApps6.Stop (Seconds (12.0));
  ApplicationContainer clientApps7 = 
  echoClient.Install (wifi2StaNodes.Get(nWifi2-3));
  clientApps7.Start (Seconds (5.0));
  clientApps7.Stop (Seconds (13.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  AsciiTraceHelper ascii;
  Simulator::Stop (Seconds (30.0));

  if(tracing == true)
  {
     phy3.EnablePcap("ns3-lec4-project4",sta3Devices.Get(0));
     phy3.EnablePcap("ns3-lec4-project4",ap3Devices.Get(0));
     phy2.EnablePcap("ns3-lec4-project4",sta2Devices.Get(0));
     phy1.EnablePcap("ns3-lec4-project4",sta1Devices.Get(0)); 

  }


  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
