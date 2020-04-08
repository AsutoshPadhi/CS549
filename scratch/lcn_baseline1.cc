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

int main (int argc, char *argv[])
{

	std::string freq="1";
	CommandLine cmd;
	cmd.AddValue("intFreq","interest frequency", freq);
	cmd.Parse(argc, argv);

	std::cout << freq;
	string producerLocation = "bb";

	uint32_t badCount = 1; //No. of malicious nodes
	uint32_t goodCount = 5; //No. of legitimate nodes




	AnnotatedTopologyReader topologyReader("", 25);
 
	topologyReader.SetFileName ("ndnSIM-sample-topologies/topologies/bw-delay-rand-1/1755.r0-conv-annotated.txt");
  	topologyReader.Read();
  	ndn::StackHelper ndnHelper;
	ndn::Interest::setDefaultCanBePrefix(0);
	//ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize","50");
  	ndnHelper.setCsSize(100);
 	ndnHelper.setPolicy("nfd::cs::lru");

  	ndnHelper.InstallAll ();


	ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route/%FD%05");
	//ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/multicast/%FD%03");

	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  	ndnGlobalRoutingHelper.InstallAll();


	NodeContainer leaves;
    NodeContainer gw;
    NodeContainer bb;
    for_each (NodeList::Begin (), NodeList::End (), [&] (Ptr<Node> node) 
	{
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

	std::cout << "Total_numbef_of_nodes      : " << NodeList::GetNNodes () << endl;
   	std::cout << "Total_number_of_leaf_nodes : " << leaves.GetN () << endl;
   	std::cout << "Total_number_of_gw_nodes   : " << gw.GetN () << endl;
   	std::cout << "Total_number_of_bb_nodes   : " << bb.GetN () << endl;

 
  
	NodeContainer evilNodes;
	NodeContainer goodNodes;
	NodeContainer monitorNodes;
	NodeContainer producerNodes;
	NodeContainer evil_producerNodes;



	std:: set< Ptr<Node> > evils;
	std:: set< Ptr<Node> > angels;
	std:: set< Ptr<Node> > monitors;
	std::set< Ptr<Node> > producers;
	std::set< Ptr<Node> > evil_producers;



 	while (evils.size () < badCount)
    {

		double min1 = 0.0;
        double max1 = leaves.GetN ();
	 	Ptr<UniformRandomVariable> x1 = CreateObject<UniformRandomVariable> ();
	 
	 	x1->SetAttribute ("Min", DoubleValue (min1));
        x1->SetAttribute ("Max", DoubleValue (max1));

	
        int node_id= int (x1->GetValue ());
     
	 	Ptr<Node> node = leaves.Get (node_id);

	 	std::cout <<"Node Id " <<node_id << "\t" << node->GetId()<< "\n";

    	if (evils.find (node) != evils.end ())
        	continue;

        evils.insert (node);
      
        string name = Names::FindName (node);

		std::cout << "\nName \t" << name << "\n";
		Names::Rename (name, "evil-"+name);
    }



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
    }






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
          	///node = bb.Get (randVar.GetValue ());
        }
      
	 	if (producers.find (node) != producers.end ())
			continue;
        producers.insert (node);

	
      	string name = Names::FindName (node);

		std::cout << "\nProducer Name :\t" << name << "\n";
      	Names::Rename (name, "producer-"+name);
    }

	//------------------------
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

		std::cout << "\nProducer Name :\t" << name << "\n";
      	Names::Rename (name, "evil_producer-"+name);
	}
	//-------------------


 	string folder = "tmp";

	string meta_file    = "results/" + folder + "/" + ".meta";	

	ofstream os (meta_file.c_str(), ios_base::out | ios_base::trunc);

	std::cout<<"\n\n\n\n";	
	auto assignNodes = [&os](NodeContainer &aset, const string &str) 
	{
	    return [&os, &aset, &str] (Ptr<Node> node)
    	{
      		string name = Names::FindName (node);

			std::cout << "*****name : " <<name << "\n";
      
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
	//Custom
	std::cout<<"\n\n\n";
	std::cout<<"Number of Good Nodes : "<<goodNodes.size()<<std::endl;
	for (NodeContainer::Iterator node = goodNodes.Begin (); node != goodNodes.End (); node++)
    {

		//Custom
		//std::cout<<"Good : "<<node->GetId()<<" "<<ns3::Names::FindName(node)<<std::endl;
	
		std::cout<<"Good : "<<(*node)->GetId()<<" "<<ns3::Names::FindName(*node)<<std::endl;
     
     	ndn::AppHelper goodAppHelper ("ns3::ndn::ConsumerZipfMandelbrot");
     
		//ndn::AppHelper goodAppHelper ("ns3::ndn::ConsumerCbr");
	
		//consumerHelper.SetPrefix ("/prefix/"+to_string(i)+"/");
		goodAppHelper .SetPrefix ("/good/"+to_string(i)+"/");
    	i++;
	
		goodAppHelper .SetAttribute ("Frequency", StringValue ("30")); // 100 interests a second

       	// goodAppHelper .Install (*node);
   		// goodAppHelper.Start (Seconds (1.0) );
    	// goodAppHelper.Stop (Seconds (6.0);

    	ApplicationContainer consumer = goodAppHelper.Install(*node);
    	consumer.Start(Seconds(1.0));     // start consumers at 0s, 1s, 2s, 3s
    	consumer.Stop(Seconds(9.0));
    }
  

	//Custom
	std::cout<<"\n";
	std::cout<<"Number of Evil Nodes : "<<evilNodes.size()<<std::endl;
	for (NodeContainer::Iterator node =evilNodes.Begin (); node != evilNodes.End (); node++)
	{
		//Custom
		//std::cout<<"Evil : "<<node->GetId()<<" "<<ns3::Names::FindName(node)<<std::endl;
		std::cout<<"Evil : "<<(*node)->GetId()<<" "<<ns3::Names::FindName(*node)<<std::endl;

		ns3::Ptr<ns3::Node> e = *node;


		ndn::AppHelper goodAppHelper ("ns3::ndn::ConsumerCbr1");// ns3::ndn::ConsumerCbr1
		
		
		goodAppHelper .SetPrefix ("/evil");
		
		goodAppHelper .SetAttribute ("Frequency", StringValue ("10")); // 100 interests a second

		//goodAppHelper .Install (*node);
		//goodAppHelper.Start (Seconds (1.0) );
		//goodAppHelper.Stop (Seconds (6.0);

		ApplicationContainer consumer = goodAppHelper.Install(*node);
		consumer.Start(Seconds(1.0));     // start consumers at 0s, 1s, 2s, 3s
		consumer.Stop(Seconds(9.0));
	}
  


	for (NodeContainer::Iterator node = producerNodes.Begin (); node != producerNodes.End (); node++)
    {

		ndn::AppHelper producerHelper ("ns3::ndn::Producer");



		producerHelper.SetPrefix("/good");

		ndnGlobalRoutingHelper.AddOrigins ("/good",*node);

		producerHelper.Install(*node);

	
         
	}

 	for (NodeContainer::Iterator node = evil_producerNodes.Begin (); node != evil_producerNodes.End (); node++)
    {
        ndn::AppHelper producerHelper ("ns3::ndn::Producer");
		producerHelper.SetPrefix("/evil");
		ndnGlobalRoutingHelper.AddOrigins ("/evil",*node);
		producerHelper.Install(*node);
	}
  
	ndnGlobalRoutingHelper.CalculateAllPossibleRoutes();



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



	std::string file_name2 = "APP_NNbottleneck_FREQ_" + freq + ".txt";


	ndn::AppDelayTracer::InstallAll(file_name2);
	
	Simulator::Stop(Seconds(20.0));
	Simulator::Run ();



	NodeContainer nl2 = NodeContainer::GetGlobal();
	uint32_t nNodes2 = nl2.GetN ();


	std::string file_name1 = "PitSize_30.txt";

	ofstream file1;
	file1.open (file_name1, std::ofstream::out | std::ofstream::app);

	for (uint32_t i1 = 0 ;i1 < nNodes2; ++i1)

	{
		Ptr<Node> thisNode2 = nl2.Get (i1);
	
		Ptr<ndn::L3Protocol> l3=ndn::L3Protocol::getL3Protocol(thisNode2);


		int max;
        long int interest, data;

        // std::tie(max, interest, data)= l3->getForwarder()->CollectData1();
		//int max=(l3->getForwarder()).CollectPitSize() << "\n";

        //std::cout <<  thisNode2->GetId() << "\t" << ns3::Names::FindName(thisNode2) << "\n";


		//file1<<  max <<"\t" << interest << "\t" << data << "\t" << "ISR=" << (float) data/interest <<  "\t"<< thisNode2->GetId() << "\t" <<  				ns3::Names::FindName(thisNode2) << "\n";
	}


	Simulator::Destroy ();


	return 0;
}

} 


int main(int argc, char* argv[])
{
	return ns3::main(argc, argv);
}
