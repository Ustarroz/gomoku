//
// Created by Mathieu on 07/12/2017.
//

#include <afxres.h>
#include <Tree.h>
#include <UCT.h>
#include <limits>
#include "MonteCarlo.h"

MonteCarlo::MonteCarlo() {}

MonteCarlo::~MonteCarlo() {

}

long MonteCarlo::getCurrentTime() {
    SYSTEMTIME time;
    GetSystemTime(&time);
    return (time.wSecond * 1000) + time.wMilliseconds;
}

const Point &MonteCarlo::findNextMove(const AI &ai, int playerNb) {

    long start = getCurrentTime();
    long end = start + 4000;
    Tree tree;
    const std::shared_ptr<Node> &root = tree.getRoot();

    opponent = 3 - playerNb;
    root->getState()->setAi(ai);
    root->getState()->setPlayer(opponent);

    while (getCurrentTime() < end) {
//        std::cout << "Time : " << getCurrentTime() << " < " << end << std::endl;
        //Phase 1 selection
        std::shared_ptr<Node> nextBestNode = findNextBestNode(root);
        //Phase 2 expansion
        expandNode(nextBestNode);

        /*    std::cout << "HERE " << nextBestNode->getState()->getAi().getPositionPlayed() << " "
                  << nextBestNode->getState()->getPlayer()
                  << " " << nextBestNode->getChildren()[0]->getState()->getAi().getPositionPlayed()
                  << " " << nextBestNode->getChildren()[0]->getState()->getPlayer() << std::endl;
*/
        //Phase 3  Simulation
        std::shared_ptr<Node> nodeToExplore = nextBestNode;
        if (!nextBestNode->getChildren().empty()) {
            nodeToExplore = nextBestNode->getRandomChildNode();
        }

        int playResult = simulatePlay(nodeToExplore);

        // Phase 4 - Update
        backPropagation(nodeToExplore, playResult);
    }
//    std::cout << "root child " << root->getChildren().size() << " root score "
//              << root->getState()->getScore() << "  " << root->getState()->getAi().getPositionPlayed() << std::endl;
    /* for (auto &child: root->getChildren()) {
         std::cout << "root child score " << child->getState()->getScore() << std::endl;

     }*/

    std::shared_ptr<Node> winnerNode = root->getChildWithMaxScore();
//    tree.setRoot(winnerNode);
    return winnerNode->getState()->getAi().getPositionPlayed();
}

const std::shared_ptr<Node> MonteCarlo::findNextBestNode(const std::shared_ptr<Node> &rootNode) {
    std::shared_ptr<Node> node = rootNode;
//    std::cout << "Finding Next best node...." << std::endl;

    while (!node->getChildren().empty()) {
//        std::cout << "Finding Next best node loop...." << std::endl;
        node = UCT::findNodeWithBestUCT(node);
    }
    return node;
}

void MonteCarlo::expandNode(const std::shared_ptr<Node> &node) {
    std::vector<std::shared_ptr<State>> allNextStates = node->getState()->getAllNextStates();

    for (auto &state: allNextStates) {
//        std::cout << "Expanding Node...." << state->getAi().getPositionPlayed() << std::endl;
        std::shared_ptr<Node> newNode = std::make_shared<Node>(state);
        newNode->setParent(node);
        newNode->getState()->setPlayer(node->getState()->getAdversary());
        node->getChildren().push_back(newNode);
        if (node->getState()->getAi().checkStatus() == AI::WIN)
            break;
    }
}

int MonteCarlo::simulatePlay(const std::shared_ptr<Node> &node) {
    std::unique_ptr<Node> tmpNode = std::make_unique<Node>(node);
    std::unique_ptr<State> tmpState = std::make_unique<State>(tmpNode->getState());

    int gameStatus = tmpState->getAi().checkStatus();
    if (gameStatus == AI::LOOSE && tmpState->getAi().getPlayerNb() == 1) {
        tmpNode->getParent()->getState()->setScore(std::numeric_limits<int>::min());
        return gameStatus;
    }
    while (gameStatus == AI::IN_PROGRESS) {
//        std::cout << "Simulating random Play..." << std::endl;
        tmpState->togglePlayerNb();
        tmpState->randomPlay();
        gameStatus = tmpState->getAi().checkStatus();
    }
//    std::cout << "game Status " << gameStatus << std::endl;
    return gameStatus;
}

void MonteCarlo::backPropagation(const std::shared_ptr<Node> &node, int playResult) {
    std::shared_ptr<Node> tmpNode = node;
    while (tmpNode != nullptr) {
//        std::cout << "Backpropagation..." << std::endl;
        tmpNode->getState()->incrementVisit();
//        std::cout << "player " << tmpNode->getState()->getPlayer() << " = " << playerNb << std::endl;
        if (tmpNode->getState()->getPlayer() == 2 && playResult == AI::WIN)
            tmpNode->getState()->addScore(WIN_SCORE);
//        std::cout << "score " << tmpNode->getState()->getScore() << " / visite " << tmpNode->getState()->getVisitCount()
//                  << " move " << tmpNode->getState()->getAi().getPositionPlayed() << std::endl;
        tmpNode = tmpNode->getParent();
    }
}