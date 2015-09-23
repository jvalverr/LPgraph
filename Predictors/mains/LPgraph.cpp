/*
------------------------------------------------
Copyright (C) 2015 by Jorge C. Valverde-Rebaza
Email: jvalverr@icmc.usp.br
This file is part of LPgraph and is based on the paper [1]

[1] L. Berton and J. Valverde-Rebaza and A. Lopes. "Link prediction in graph construction for supervised
    and semi-supervised learning". The 2015 International Joint Conference on Neural Networks (IJCNN 2015),
    pp. 1818 - 1825, IEEE, 2015.	

LPgraph is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include <iostream>

#include "../../Network.h"


//traditional structural measures
#include "../CommonNeighborsLP.h"
#include "../AdamicAdarLP.h"
#include "../JaccardLP.h"
#include "../ResourceAllocationLP.h"
#include "../PreferentialAttachmentLP.h"

//weighted traditional measures
#include "../WeightedCommonNeighborsLP.h"
#include "../WeightedAdamicAdarLP.h"
#include "../WeightedJaccardLP.h"
#include "../WeightedPreferentialAttachmentLP.h"
#include "../WeightedResourceAllocationLP.h"

//global traditional measures
#include "../KatzLP.h"
#include "../SimRankLP.h"
#include "../RootedPageRankLP.h"


//overlapping group measures (proposals)
#include "../WithinAndOutsideCommonGroupsLP.h"
#include "../CommonNeighborsOfGroupsLP.h"
#include "../TotalAndPartialOverlappingOfGroupsLP.h"



void graphConstructionProcess(const char*);
void LPgraph( vector<Link>, vector<Link>, const char*);
vector<Link> linkPredictionProcess(const Network&, const char*);
vector<Link> mergeLinksLists ( vector<Link>, vector<Link>, unsigned int);
vector<string> getLinkPredictionMethods();



using namespace std;



int main(){
    const char* initialGraphName = "../data/initialGraph.con";
    graphConstructionProcess(initialGraphName);
    return 0;
}

void graphConstructionProcess(const char* graphNameFile){
    cout << "\n Start the process";
    Network network = Network();
    cout << "\n Reading weighted links file: " << graphNameFile;
    network.readWeightedLinksFile( graphNameFile );

    vector<string> listMethods = getLinkPredictionMethods();
    cout << "\n Going to calculate the predictors";
    for(unsigned int i = 0; i < listMethods.size(); i++){
        char outFileName[800];
        strcpy( outFileName   , "../output/");
        strcat( outFileName   , listMethods[i].c_str() );

        vector<Link> predictedLinksList = linkPredictionProcess( network, listMethods[i].c_str());
        if(predictedLinksList.size() > 0)
            LPgraph(network.getTrainLinks(), predictedLinksList, outFileName);
        else
            cerr << "\n Error in the process";
    }
    cout << "\n Process finished\n";

}

void LPgraph( vector<Link> trainLinksList, vector<Link> predictedLinksList, const char* outFile ){
    unsigned int totalPredictedLinks = predictedLinksList.size();
    vector<string> listPercent;
    listPercent.push_back("10");
    listPercent.push_back("20");
    listPercent.push_back("30");
    listPercent.push_back("40");
    listPercent.push_back("50");
    listPercent.push_back("60");
    listPercent.push_back("70");
    listPercent.push_back("80");
    listPercent.push_back("90");
    listPercent.push_back("100");


    //sort predicted links by score
    sort(predictedLinksList.begin(), predictedLinksList.end(), scoreLinkCompare());

    for(unsigned iTop = 0; iTop < listPercent.size(); iTop++ ){
        char outFileName[800];
        strcpy( outFileName   , outFile );
        strcat( outFileName   , "-" );
        strcat( outFileName   , "Top" );
        strcat( outFileName   , listPercent[iTop].c_str() );
        strcat( outFileName   , ".lpg" );
        cout << "\n Creating the graph file: " << outFileName;

        ofstream out;
        out.open( outFileName );
        unsigned int top = (iTop+1) * totalPredictedLinks / 10;
        vector<Link> mergeList = mergeLinksLists ( trainLinksList, predictedLinksList, top );

        //create a empty network to use specific functions on test links but on our merge links list
        Network emptyNet;
        emptyNet.setTestLinksList(mergeList); //set our merge links list as test links in the empty network
        emptyNet.sortLinkListTest(); //now we can use the sorter function
        emptyNet.printWeightedLinksTest(out);  //and also we can use the print function
        out.close();
    }
}

vector<Link> mergeLinksLists ( vector<Link> trainLinksList, vector<Link> predictedLinksList, unsigned int top ){
    vector<Link> mergeList;
    for(unsigned int i = 0; i < trainLinksList.size(); i++){
        mergeList.push_back(trainLinksList[i]);
    }

    unsigned int cont = 0;
    for(unsigned int i = 0; i < predictedLinksList.size(); i++){
        if(cont >= top)
            break;

        mergeList.push_back(predictedLinksList[i]);
        cont++;
    }
    return mergeList;
}



vector<Link> linkPredictionProcess(const Network& network, const char* predictorName){
    cout << "\n Link prediction method: " << predictorName;
    LinkPredictor* predictor = NULL;
    int lengthPath = 5;
    double beta = 0.05;
    double alpha = 0.15;
    double C = 0.8;
    bool predictorExist = true;

    if( strcmp( predictorName, "CommonNeighbors" ) == 0 ) {
        predictor = new CommonNeighborsLP( network );
     }
     else if( strcmp( predictorName, "AdamicAdar" ) == 0 ) {
        predictor = new AdamicAdarLP( network );
     }
     else if( strcmp( predictorName, "Jaccard" ) == 0 ) {
        predictor = new JaccardLP( network );
     }
     else if( strcmp( predictorName, "ResourceAllocation" ) == 0 ) {
        predictor = new ResourceAllocationLP( network );
     }
     else if( strcmp( predictorName, "PreferentialAttachment" ) == 0 ) {
        predictor = new PreferentialAttachmentLP( network );
     }
     else if( strcmp( predictorName, "CommonNeighborsOfGroups" ) == 0 ) {
        predictor = new CommonNeighborsOfGroupsLP( network );
     }
     else if( strcmp( predictorName, "TotalAndPartialOverlappingOfGroups" ) == 0 ) {
        predictor = new TotalAndPartialOverlappingOfGroupsLP( network );
     }
     else if( strcmp( predictorName, "WithinAndOutsideCommonGroups" ) == 0 ) {
        predictor = new WithinAndOutsideCommonGroupsLP( network );
     }
     else if( strcmp( predictorName, "WeightedCommonNeighbors" ) == 0 ) {
        predictor = new WeightedCommonNeighborsLP( network );
    }
    else if( strcmp( predictorName, "WeightedAdamicAdar" ) == 0 ) {
        predictor = new WeightedAdamicAdarLP( network );
    }
    else if( strcmp( predictorName, "WeightedJaccard" ) == 0 ) {
        predictor = new WeightedJaccardLP( network );
    }
    else if( strcmp( predictorName, "WeightedResourceAllocation" ) == 0 ) {
        predictor = new WeightedResourceAllocationLP( network );
	}
	else if( strcmp( predictorName, "WeightedPreferentialAttachment" ) == 0 ) {
	    predictor = new WeightedPreferentialAttachmentLP( network );
	}
	else if( strcmp( predictorName, "Katz" ) == 0 ) {
        predictor = new KatzLP( network , lengthPath, beta );
    }
    else if( strcmp( predictorName, "SimRank" ) == 0 ) {
        predictor = new SimRankLP( network, C );
	}
	else if( strcmp( predictorName, "RootedPageRank" ) == 0 ) {
	    predictor = new RootedPageRankLP( network, alpha );
	}
    else {
        cout<<"\n Predictor " << predictorName << " does not exist";
        predictorExist = false;
    }
    if( predictorExist == true ){
        unsigned int degree = 2;
        return predictor->getListScores( degree );
    }
    vector<Link> emptyList;
    return emptyList;
}


vector<string> getLinkPredictionMethods(){
    vector<string> listMethods;
    listMethods.push_back( "CommonNeighbors" );
    listMethods.push_back( "AdamicAdar" );
    listMethods.push_back( "Jaccard" );
    listMethods.push_back( "ResourceAllocation" );
    listMethods.push_back( "PreferentialAttachment" );

    listMethods.push_back( "WeightedCommonNeighbors" );
    listMethods.push_back( "WeightedAdamicAdar" );
    listMethods.push_back( "WeightedJaccard" );
    listMethods.push_back( "WeightedResourceAllocation" );
    listMethods.push_back( "WeightedPreferentialAttachment" );

    listMethods.push_back( "Katz" );
    listMethods.push_back( "SimRank" );
    listMethods.push_back( "RootedPageRank" );
    return listMethods;
}
