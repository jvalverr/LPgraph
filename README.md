# LPgraph
LPgraph is a framework for graph construction based on link prediction following the technique proposed in [1]. LPgraph framework works with LPsource framework, which provides a set of link prediction measures ready to be used.

Based on an initial graph, which should be constructed using some traditional graph construction technique. After, some link
prediction method is computed with the objective of estimate new links in the graph generating a new more balanced graph. Experiments showed in [1] determine that LPgraph technique improves the quality of graphs leading to better classification accuracy in supervised and semi-supervised domains.

The following steps are necessary to build LPgraph and prepare it for use:

1. Go to LPsource project (here: https://github.com/jvalverr/LPsource) and download it
2. Save the initial graph to be processed into the directory ./data
3. Replace the Makefile of LPsource project by the MakeFile of LPgraph project
4. Save the file ./Predictors/mains/LPgraph.cpp of LPgraph project into the directory ./Predictors/mains/
5. Compile the code using the comand 'make'. The Makefile will run and will save the binary into the directory ./bin
5. Go to directory ./bin and execute the LPgraph binary using directly the command './LPgraph' or 'nohup ./LPgraph > LPgraph.log &' to execute the binary as background (recommended for large scale graphs)
6. After the execution, find into the directory ./output the ouput files (ten for each link prediction measure) containing, each one, a network built with the initial links plus the top 10% , 20%, ... and 100% of the predicted links.

[1] L. Berton and J. Valverde-Rebaza and A. Lopes. "Link prediction in graph construction for supervised
    and semi-supervised learning". The 2015 International Joint Conference on Neural Networks (IJCNN 2015),
    pp. 1818 - 1825, IEEE, 2015.	
 

