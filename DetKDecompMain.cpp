// det-k-decomp V1.0
//
// Reference paper: G. Gottlob and M. Samer,
// A Backtracking-Based Algorithm for Computing Hypertree-Decompositions,
// Submitted for publication.
//
// Note: This program is a prototype implementation and does in no sense
// claim to be the most efficient way of implementing det-k-decomp. Moreover,
// several parts of the code have been developed within an implementation
// framework for evaluating several decomposition algorithms. These parts of 
// the code may therefore be unnecessary or are formulated in a more general 
// way than would be necessary for det-k-decomp.

// det-k-decomp V2.0
//
// Reference paper: G. Gottlob and M. Samer,
// A Backtracking-Based Algorithm for Computing Hypertree-Decompositions,
// Submitted for publication.
//
// Note: This program is a prototype implementation and does in no sense
// claim to be the most efficient way of implementing det-k-decomp. Moreover,
// several parts of the code have been developed within an implementation
// framework for evaluating several decomposition algorithms. These parts of 
// the code may therefore be unnecessary or are formulated in a more general 
// way than would be necessary for det-k-decomp.

#define _CRT_SECURE_NO_DEPRECATE

#include <cmath>
#include <ctime>
//#include <signal.h>
//#include <unistd.h>
#include <iostream>

using namespace std;

#include "PACEParser.h"
#include "Hypergraph.h"
#include "Hypertree.h"
#include "Globals.h"
#include "DetKDecomp.h"
#include "BalKDecomp.h"

HypertreeSharedPtr decompK(HypergraphSharedPtr &, int);
HypertreeSharedPtr gDecompK(HypergraphSharedPtr &, int);

//volatile sig_atomic_t tle = 0;
/*void term(int signum) {
 tle = 1;
 }*/

//volatile int z = 0;
bool cut = false;
bool heur = true;

int main(int argc, char **argv) {
	// Initialize random number generator
	int iRandomInit;
	srand((unsigned int) time(NULL));
	iRandomInit = random_range(999, 9999);
	for (int i = 0; i < iRandomInit; i++)
		rand();

	/*
	 void (*prev_handler)(int);
	 prev_handler = signal(SIGTERM, term);
	 */

	// Build hypergraph
	PACEParser *p = new PACEParser();
	HypergraphSharedPtr hg = p->parseInput(&cin);

	int n = hg->getNbrOfVertices();
	int m = hg->getNbrOfEdges();
	/*cout << "HG " << n << " " << m << endl;
	 for (auto he : HG->allEdges()) {
	 cout << he->getId() << ";" << he->getLabel() << ";" << he->getName()
	 << ": ";
	 for (auto v : he->allVertices()) {
	 cout << "(" << v->getId() << ";" << v->getName() << ")";
	 }
	 cout << endl;
	 }*/

	const int NUM_TRIES = 3;
	int tries = NUM_TRIES;
	cut = false;
	heur = true;
	//bool isHD = false;
	int low = 0, up = (int) ceil(m / 2.0);
	int k = up;
	HypertreeSharedPtr ght = NULL;
	HypertreeSharedPtr bestHT = decompK(hg, k);
	HypertreeSharedPtr ht = bestHT;
	up = bestHT->getHTreeWidth();
	k = up - 1;

	const int PHASE1 = 0;
	const int PHASE2 = 1;
	const int PHASE3 = 2;
	const int FINISHED = 3;
	int state = PHASE1;
	while (state != FINISHED && low != up && k > 0) {
		//cout << state << k << endl;
		switch (state) {
		case PHASE1: {
			cut = false;
			tries--;
			ht = decompK(hg, k);
			if (ht != NULL) {
				tries = NUM_TRIES;
				bestHT = ht;
				up = bestHT->getHTreeWidth();
				k = up - 1;
			} else {
				if (cut) {
					if (tries > 0) {
						for (int i = 0; i < iRandomInit; i++)
							rand();
					} else {
						heur = false;
						if (k <= 5) {
							state = PHASE2;
						} else {
							state = PHASE3;
						}
					}
				} else {
					low = up;
					state = FINISHED;
				}
			}
			break;
		}
		case PHASE2: {
			//isHD = false;
			ght = gDecompK(hg, k);
			if (ght == NULL) {
				low = up;
				state = FINISHED;
			} else {
				state = PHASE3;
				/*
				if (isHD) {
					bestHT = ght;
					up = bestHT->getHTreeWidth();
					k = up - 1;
				} else {
					//low = ght->getHTreeWidth();
					state = PHASE3;
				}
				*/
			}
			break;
		}
		case PHASE3: {
			ht = decompK(hg, k);
			if (ht != NULL) {
				bestHT = ht;
				up = bestHT->getHTreeWidth();
				k = up - 1;
			} else {
				low = up;
				state = FINISHED;
			}
			break;
		}
		case FINISHED: {
			break;
		}
		default: {
			cerr << "error" << endl;
		}
		}
	}
	p->writeOutput(bestHT, n, m);
	//cout << "time= " << difftime(t2, t1) << "s" << endl;
	//cout << "tot= " << difftime(fine, inizio) << "s" << endl;
	//cout << "w= " << bestHT->getHTreeWidth() << endl;

	delete p;
	return EXIT_SUCCESS;
}

HypertreeSharedPtr decompK(HypergraphSharedPtr &HG, int iWidth) {
	HypertreeSharedPtr HT;
	DetKDecomp Decomp(HG, iWidth, false);

// Apply the decomposition algorithm
	HT = Decomp.buildHypertree();
	if (HT != NULL)
		HT->shrink(false);

	return HT;
}

HypertreeSharedPtr gDecompK(HypergraphSharedPtr &HG, int iWidth) {
	HypertreeSharedPtr HT;
	BalKDecomp Decomp(HG, iWidth);
	BalKDecomp::init(HG, 0);

	// Apply the decomposition algorithm
	HT = Decomp.buildHypertree();

	return HT;
}
