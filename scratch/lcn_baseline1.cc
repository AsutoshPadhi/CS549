#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/random-variable-stream.h"
#include <iostream>
#include <fstream>
#include "ns3/node.h"
#include "ns3/node-list.h"
#include "ns3/names.h"
#include <iostream> 
#include <iterator> 
#include <map> 
using namespace std;
namespace ns3 {
int
main (int argc, char *argv[])
{
    /**
     * @brief Parameters
     */
    std::string freq="1";
    CommandLine cmd;
    cmd.AddValue("intFreq","interest frequency", freq);
    cmd.Parse(argc, argv);
    /* can be changed to bb or gw */
    string producerLocation = "gw";
    uint32_t badCount = 1; //No. of malicious nodes
    uint32_t goodCount = 5; //No. of legitimate nodes
    AnnotatedTopologyReader topologyReader("", 25);
 
    /**
     * @brief Read the topology
     */
    topologyReader.SetFileName ("ndnSIM-sample-topologies/topologies/bw-delay-rand-1/att.txt");
    topologyReader.Read();
    
    
    /**
     * @brief Configure the cache and install
     */
    ndn::StackHelper ndnHelper;
    ndn::Interest::setDefaultCanBePrefix(0);
    //ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize","50");
    ndnHelper.setCsSize(1);
    ndnHelper.setPolicy("nfd::cs::lru");
    ndnHelper.InstallAll ();
    ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route/%FD%05");
    //ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/multicast/%FD%03");
    ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
    ndnGlobalRoutingHelper.InstallAll();
    
    
    /**
     * @brief Assign good leaves, gateways and producers to NodeContainers
     */
    NodeContainer leaves;
    NodeContainer gw;
    NodeContainer bb;
    for_each (NodeList::Begin (), NodeList::End (), [&] (Ptr<Node> node) {
        if (Names::FindName (node).compare (0, 5, "leaf-")==0)
        {
            leaves.Add (node);
        }
        else if (Names::FindName (node).compare (0, 3, "gw-")==0)
        {
            gw.Add (node);
        }
        else if (Names::FindName (node).compare (0, 3, "bb-")==0)
        {
            bb.Add (node);
        }
    });

    std::cout << "Total_numbef_of_nodes      :\t" << NodeList::GetNNodes () << endl;
    std::cout << "Total_number_of_leaf_nodes :\t" << leaves.GetN () << endl;
    std::cout << "Total_number_of_gw_nodes   :\t" << gw.GetN () << endl;
    std::cout << "Total_number_of_bb_nodes   :\t" << bb.GetN () << endl;
 
  
    /**
     * @brief List of evil nodes, good nodes and producer nodes
     * 
     */
    NodeContainer evilNodes;
    NodeContainer goodNodes;
    NodeContainer producerNodes;
    NodeContainer evil_producerNodes;
 
    std::set< Ptr<Node> > evils;
    std::set< Ptr<Node> > angels;
    std::set< Ptr<Node> > producers;
    std::set< Ptr<Node> > evil_producers;
    

    /**
     * @brief : Assign an evil node
     */
    cout<<"\nEvil Node :- "<<endl;
    while (evils.size () < badCount)
    {
        double min1 = 0.0;
        double max1 = leaves.GetN ();
        Ptr<UniformRandomVariable> x1 = CreateObject<UniformRandomVariable> ();
     
        x1->SetAttribute ("Min", DoubleValue (min1));
        x1->SetAttribute ("Max", DoubleValue (max1));
    
        int node_id= int (x1->GetValue ());
     
        Ptr<Node> node = leaves.Get (node_id);
        // cout<<"Node Id : " <<node_id << "\t" << node->GetId()<< "\n";
        if (evils.find (node) != evils.end ())
            continue;
        evils.insert (node);
      
        string name = Names::FindName (node);
        Names::Rename (name, "evil-"+name);
        cout<<"Name :\t" << name << "\n";
    }


    /**
     * @brief : Assign Good nodes
     */
    cout<<"\nGood Nodes :- "<<endl;
    while (angels.size () < goodCount)
    {
        double min1 = 0.0;
        double max1 = leaves.GetN ();
        Ptr<UniformRandomVariable> x1 = CreateObject<UniformRandomVariable> ();
     
        x1->SetAttribute ("Min", DoubleValue (min1));
        x1->SetAttribute ("Max", DoubleValue (max1));
    
        int node_id= int (x1->GetValue ());
     
        Ptr<Node> node = leaves.Get (node_id);
        //UniformVariable randVar (0, leaves.GetN ());
        //Ptr<Node> node = leaves.Get (randVar.GetValue ());
        if (angels.find (node) != angels.end () || evils.find (node) != evils.end ())
            continue;
      
        angels.insert (node);
        string name = Names::FindName (node);
        Names::Rename (name, "good-"+name);
        cout<<"Name : "<<name<<endl;
    }


    /**
     * @brief : Assign Producers
     */
    cout<<"\nProducers :- "<<endl;
    while (producers.size () < 5)
    {
        Ptr<Node> node ;
        if (producerLocation == "gw")
        {
            double min1 = 0.0;
            double max1 = gw.GetN ();
            Ptr<UniformRandomVariable> x1 = CreateObject<UniformRandomVariable> ();
     
            x1->SetAttribute ("Min", DoubleValue (min1));
            x1->SetAttribute ("Max", DoubleValue (max1));
    
            int node_id= int (x1->GetValue ());
     
            node = gw.Get (node_id);
            //UniformVariable randVar (0, gw.GetN ());
            //node = gw.Get (randVar.GetValue ());
        }
        else if (producerLocation == "bb")
        {
            double min1 = 0.0;
            double max1 = bb.GetN ();
            Ptr<UniformRandomVariable> x1 = CreateObject<UniformRandomVariable> ();
     
            x1->SetAttribute ("Min", DoubleValue (min1));
            x1->SetAttribute ("Max", DoubleValue (max1));
    
            int node_id= int (x1->GetValue ());
     
            node = bb.Get (node_id);
            //UniformVariable randVar (0, bb.GetN ());
            //node = bb.Get (randVar.GetValue ());
        }
        else
        {
            cout<<"couldn't find any other node"<<endl;
        }
      
        if (producers.find (node) != producers.end ())
            continue;
        producers.insert (node);
    
        string name = Names::FindName (node);
        Names::Rename (name, "producer-"+name);
        cout << "Producer Name : " << name << "\n";
    }

    
    /**
     * @brief : Assign evil producers
     */
    cout<<"\nEvil Producers :- "<<endl;
    while (evil_producers.size () < 1)
    {
        Ptr<Node> node ;
        if (producerLocation == "gw")
        {
            double min1 = 0.0;
            double max1 = gw.GetN ();
            Ptr<UniformRandomVariable> x1 = CreateObject<UniformRandomVariable> ();
     
            x1->SetAttribute ("Min", DoubleValue (min1));
            x1->SetAttribute ("Max", DoubleValue (max1));
    
            int node_id= int (x1->GetValue ());
     
            node = gw.Get (node_id);
            //UniformVariable randVar (0, gw.GetN ());
            //node = gw.Get (randVar.GetValue ());
        }
        else if (producerLocation == "bb")
        {
            double min1 = 0.0;
            double max1 = bb.GetN ();
            Ptr<UniformRandomVariable> x1 = CreateObject<UniformRandomVariable> ();
     
            x1->SetAttribute ("Min", DoubleValue (min1));
            x1->SetAttribute ("Max", DoubleValue (max1));
    
            int node_id= int (x1->GetValue ());
     
            node = bb.Get (node_id);
            //UniformVariable randVar (0, bb.GetN ());
            //node = bb.Get (randVar.GetValue ());
        }
      
        if (evil_producers.find (node) != evil_producers.end ())
            continue;
        evil_producers.insert (node);
    
        string name = Names::FindName (node);
        Names::Rename (name, "evil_producer-"+name);
        cout << "Producer Name : " << name << "\n";
    }


    //-------------------
    cout<<"\n\n";
    string folder = "tmp";
    string meta_file    = "results/" + folder + "/" + ".meta";
    ofstream os (meta_file.c_str(), ios_base::out | ios_base::trunc);
    auto assignNodes = [&os](NodeContainer &aset, const string &str) {
        return [&os, &aset, &str] (Ptr<Node> node)
        {
            string name = Names::FindName (node);
            cout << "*****name : " <<name << "\n";
            aset.Add (node);
        };
    };
    os << endl;
 
    std::for_each (evils.begin (), evils.end (), assignNodes (evilNodes, "Evil"));
    std::for_each (angels.begin (), angels.end (), assignNodes (goodNodes, "Good"));
  
    std::for_each (producers.begin (), producers.end (), assignNodes (producerNodes, "Producers"));
    std::for_each (producers.begin (), producers.end (), assignNodes (evil_producerNodes, "evil_Producers"));
  
    //ndn::AppHelper goodAppHelper ("ns3::ndn::ConsumerCbr");
    //ndn::AppHelper evilAppHelper ("ns3::ndn::ConsumerCbr");
    int i=0;
    for (NodeContainer::Iterator node = goodNodes.Begin (); node != goodNodes.End (); node++)
    {
        cout<<"Good Nodes : "<<endl;
        ndn::AppHelper goodAppHelper ("ns3::ndn::ConsumerZipfMandelbrot");
     
        //ndn::AppHelper goodAppHelper ("ns3::ndn::ConsumerCbr");
    
        //consumerHelper.SetPrefix ("/prefix/"+to_string(i)+"/");
        goodAppHelper .SetPrefix ("/good/"+to_string(i)+"/");
        i++;
    
        goodAppHelper .SetAttribute ("Frequency", StringValue ("5")); // 100 interests a second
        // goodAppHelper .Install (*node);
        //goodAppHelper.Start (Seconds (1.0) );
        //goodAppHelper.Stop (Seconds (6.0);
        ApplicationContainer consumer = goodAppHelper.Install(*node);
        consumer.Start(Seconds(1.0));     // start consumers at 0s, 1s, 2s, 3s
        consumer.Stop(Seconds(8.0));
    }
  
    /*for (NodeContainer::Iterator node =evilNodes.Begin (); node != evilNodes.End (); node++)
    {
        
        ndn::AppHelper goodAppHelper ("ns3::ndn::ConsumerCbr1");// ns3::ndn::ConsumerCbr1
        
        
        goodAppHelper .SetPrefix ("/evil");
        
        //goodAppHelper .SetAttribute ("Frequency", StringValue (freq)); // 100 interests a second
        // goodAppHelper .Install (*node);
        //goodAppHelper.Start (Seconds (1.0) );
        //   goodAppHelper.Stop (Seconds (6.0);
        ApplicationContainer consumer = goodAppHelper.Install(*node);
        consumer.Start(Seconds(1.0));     // start consumers at 0s, 1s, 2s, 3s
        consumer.Stop(Seconds(10.0));
    }*/
  

    for (NodeContainer::Iterator node = producerNodes.Begin (); node != producerNodes.End (); node++)
    {
        cout<<"Good Producer : "<<endl;
        ndn::AppHelper producerHelper ("ns3::ndn::Producer");
        producerHelper.SetPrefix("/good");
        ndnGlobalRoutingHelper.AddOrigins ("/good",*node);
        producerHelper.Install(*node);
    }


    for (NodeContainer::Iterator node = evil_producerNodes.Begin (); node != evil_producerNodes.End (); node++)
    {
        cout<<"Evil Producer : "<<endl;
        ndn::AppHelper producerHelper ("ns3::ndn::Producer");
        producerHelper.SetPrefix("/evil");
        ndnGlobalRoutingHelper.AddOrigins ("/evil",*node);
        producerHelper.Install(*node);
    }
  
    /*int i=0;
    for (NodeContainer::Iterator node = leaves.Begin (); node != leaves.End (); node++)
    {
    
       
        ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
    
    
        consumerHelper.SetPrefix ("/prefix"+to_string(i)+"/");
        
        consumerHelper.SetAttribute ("Frequency", StringValue (freq)); // 100 interests a second
            consumerHelper.Install (*node);
        i++;
    }
    i=0;
    for (NodeContainer::Iterator node = bb.Begin (); node != bb.End (); node++)
    {
        ndn::AppHelper producerHelper ("ns3::ndn::Producer");
        //producerHelper.SetPrefix ("/prefix"+to_string(i)+"/");
        producerHelper.SetPrefix("/prefix");
        ndnGlobalRoutingHelper.AddOrigins ("/prefix",*node);
        producerHelper.Install(*node);
        i++; 
    }*/

    // ndn::GlobalRoutingHelper::CalculateRoutes();


    /**
     * @brief : Run the simulator
     */
    ndnGlobalRoutingHelper.CalculateAllPossibleRoutes();
    std::string file_name2 = "APP_NNbottleneck_FREQ_" + freq + ".txt";
    ndn::AppDelayTracer::InstallAll(file_name2);
    Simulator::Stop(Seconds(20.0));
    Simulator::Run ();
    Simulator::Destroy ();
 
 
    return 0;
}
}


int
main(int argc, char* argv[])
{
    return ns3::main(argc, argv);
}