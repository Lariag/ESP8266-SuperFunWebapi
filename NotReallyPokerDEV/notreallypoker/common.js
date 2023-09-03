let connection;
let isConnected = false;
let myId = 0;
let loginContainer;
let gameContainer;
let loadingLayer;
let errorLayer;
let errorText;
let playerCards;
let playerCardsContainer;
let tableCardsContainer;
let staticsContainer;

let btnNewGame;
let btnRevealCards;

let allPlayers = new Array(); // Properties: id, name, card. 
let areCardsRevealed = false
let isRevealCardsSent = false;
let isExpectator = false;

let loadingStack = 0;

function switchScreen(showLogin) {

    if (showLogin) {
        const urlParams = new URLSearchParams(window.location.search);
        const inputTable = document.getElementById('inputTable');
        const inputPlayer = document.getElementById('inputPlayer');

        inputTable.value = urlParams.get('table');
        inputPlayer.value = urlParams.get('player');

        if (inputTable.value.length === 0) {
            inputTable.focus();
        } else if (inputPlayer.value.length === 0) {
            inputPlayer.focus();
        }

        loginContainer.style.display = 'block';
        logoImage.style.display = 'block';
        gameContainer.style.display = 'none';
    } else {
        loginContainer.style.display = 'none';
        gameContainer.style.display = 'block';
        logoImage.style.display = 'none';
        if (!isExpectator) {
            switchBottomPanel(null, playerCards);
            setTimeout(generateAndShowPlayerCards, 410);
        }
    }
}

function showLoading(isLoading) {
    loadingStack += isLoading ? 1 : -1;
    if (isLoading) {
        loadingLayer.style.display = 'block';
    } else if (loadingStack <= 0) {
        loadingStack = 0;
        loadingLayer.style.display = 'none';
    }
}

function showError(text) {
    if (text === false) {
        errorLayer.style.display = 'none';
        location.reload();
    } else {
        errorLayer.style.display = 'block';
        errorText.textContent = text;
    }
}

function showLoginError(text) {
    loginError = document.getElementById('loginError');
    loginError.textContent = text;
    loginError.style.display = text.length == 0 ? 'none' : 'block'
}

function webSocketConect() {
    showLoading(true);
    connection = new WebSocket('ws:' + location.hostname + ':8087/', ['arduino']);
    //connection = new WebSocket('ws:192.168.1.186:8088', ['arduino']);
    connection.onerror = function (error) {
        showError(`Error: ${error}`);
        isConnected = false;
        console.log('WebSocket Error, ERROR!!! WHY?!?! ', error);
    };

    connection.onclose = function () {
        switchScreen(true);
        myId = -1;
        isConnected = false;
        showLoading(false);
        console.log('Connection closed');
        setTimeout(() => showError('Connection lost.'), 500);
    };
    connection.onmessage = function (event) { messageReceived(event); };

    function messageReceived(evt) {
        console.log('MESSAGE: ' + evt.data);
        let msg = JSON.parse(evt.data);

        showLoading(false);
        if (msg.yourId !== undefined) {
            console.log('Connection open. Id: ' + myId);
            isConnected = true;
            myId = msg.yourId;
            switchScreen(true);
        } else if (msg.disconnectedId !== undefined) {
            playerDisconnected(msg.disconnectedId);
        } else if (msg.connectedId !== undefined) {
            if (msg.connectedId != myId) {
                playerConnected(msg.connectedId, msg.name);
            }
        } else if (msg.a !== undefined) {
            const action = msg.a;
            switch (action) {
            case 1: // Result of a login.
                if (msg.tp !== undefined) {
                    switchScreen(false);
                    allPlayers = new Array();
                    msg.tp.forEach((val, index, array) => {
                        allPlayers.push({
                            'id': val.id,
                            'name': val.n,
                            'card': val.c !== undefined ? val.c : 0
                        });
                    });
                } else if (msg.n !== undefined && msg.id != myId) {
                    allPlayers.push({
                        'id': msg.id,
                        'name': msg.n,
                        'card': msg.c !== undefined ? msg.c : 0
                    });
                }

                RearangeTableCards();
                break;
            case 2: // Confirmation that card has been choosen.
                if (msg.id == myId) {
                    playerCardSelected(msg.c);
                } else {
                    tableCardSelected(msg.id, msg.c);
                }
                break;
            case 3: // Cards revelation.
                if (msg.tc !== undefined) {
                    msg.tc.forEach((val, index, array) => {
                        const p = allPlayers.find(x => x.id === val.id);
                        if (p) {
                            p.card = val.c;
                        }
                    });
                    receivedRevealTableCards();
                }
                break;
            case 4:
                receivedResetGame();
                break;
            }

        } else if (msg.e !== undefined && msg.e > 0) {
            let errorText = 'Unexpected Error.';

            switch (msg.e) {
            case 1:
                errorText = 'Player name is already present in the table';
                break;
            case 2:
                errorText = 'Player already in another table or all slots are full.';
                break;
            case 30:
                errorText = 'The process to choose a card failed.';
                break;
            case 31:
                errorText = 'The process to reset players cards failed.'
                break;
            case 104:
            case 105:
                errorText = 'Invalid data was sent.';
                break;
            }

            if (msg.e < 10) {
                showLoginError(errorText);
            } else {
                showError(errorText);
            }
        }
    };
}

function sendJson(jsonObject, loadingScreen = false) {
    if (loadingScreen) {
        showLoading(true);
    }
    let msgJSON = JSON.stringify(jsonObject);
    console.log('SendingMessage: ' + msgJSON);
    connection.send(msgJSON);
}

function sendLogin(asPlayer) {

    if (inputTable.value === undefined || inputPlayer.value === undefined ||
        inputTable.value.length == 0 || inputPlayer.value.length == 0) {
        showLoginError('Both table and user must be introduced.');
        return;
    }
    if (inputTable.value.length > 15 || inputPlayer.value.length > 15) {
        showLoginError('Both player and table name must be 15 characters or shorter.');
        return;
    }

    isExpectator = !asPlayer;
    document.getElementById('tableName').firstElementChild.textContent = `${inputTable.value} - ${inputPlayer.value}`;
    let loginMessage = { action: 1, table: inputTable.value, player: inputPlayer.value, expectator: !asPlayer };
    sendJson(loginMessage, true);
}

function sendCardSelected(cardSelected) {
    let cardSelectedMessage = { action: 2, cardNumber: cardSelected };
    sendJson(cardSelectedMessage);
}

function sendRevealCards() {
    let revealCardsMessage = { action: 3 };
    sendJson(revealCardsMessage);
}

function sendGameReset() {
    let resetGameMessage = { action: 4 };
    sendJson(resetGameMessage);
}

function clickCardRevelation() {
    if (btnRevealCards.classList.contains('btnConfirmation')) {
        btnRevealCards.classList.remove('btnConfirmation');
        btnRevealCards.setAttribute('disabled', 'disabled');
        btnRevealCards.style.display = 'none';
        sendRevealCards();
        isRevealCardsSent = true;
    } else {
        btnRevealCards.classList.add('btnConfirmation');
        setTimeout(() => {
            btnRevealCards.classList.remove('btnConfirmation');
        }, 1100);
    }
}

function clickGameReset() {
    if (btnNewGame.classList.contains('btnConfirmation')) {
        btnNewGame.classList.remove('btnConfirmation');
        btnNewGame.setAttribute('disabled', 'disabled');
        btnNewGame.style.display = 'none';
        sendGameReset();
    } else {
        btnNewGame.classList.add('btnConfirmation');
        setTimeout(() => {
            btnNewGame.classList.remove('btnConfirmation');
        }, 1100);
    }
}

function receivedResetGame() {
    isRevealCardsSent = false;
    areCardsRevealed = false;

    // Exhange buttons.
    btnNewGame.style.display = 'none';
    btnRevealCards.style.display = 'block';
    btnRevealCards.removeAttribute('disabled');

    // Hide result.

    // Reset cards.
    allPlayers.forEach(p => {
        if (p.card > 0) {
            p.card = 0;
        }
    });

    // Switch bottom content.
    switchBottomPanel(staticsContainer, isExpectator ? null : playerCards);

    RearangeTableCards();
    setTimeout(generateAndShowPlayerCards, 410);
}

function receivedRevealTableCards() {
    if (areCardsRevealed) {
        return;
    }
    areCardsRevealed = true;

    btnRevealCards.style.display = 'block';
    btnNewGame.style.display = 'none';
    btnRevealCards.removeAttribute('disabled');


    // Exhange buttons.
    btnRevealCards.style.display = 'none';
    btnNewGame.style.display = 'block';
    btnNewGame.removeAttribute('disabled');

    // Switch bottom content.
    switchBottomPanel(isExpectator ? null : playerCards, staticsContainer);

    // Show result.
    calculateStatics();
    RearangeTableCards();
}

function switchBottomPanel(panelToHide, panelToShow) {
    if (panelToHide != null) {
        panelToHide.style.opacity = 0;
        panelToHide.style.bottom = '-100px';
        setTimeout(() => {
            panelToHide.style.display = 'none';

        }, 500);
    }

    if (panelToShow != null) {
        setTimeout(() => {
            panelToShow.style.display = 'block';
            setTimeout(() => {
                panelToShow.style.bottom = '0';
                panelToShow.style.opacity = 1;
            }, 1);
        }, 400);
    }
}

function playerDisconnected(playerId) {
    RemoveTableCard(playerId);
    allPlayers = allPlayers.filter(player => player.id != playerId);
    RearangeTableCards();
}

function playerConnected(playerId, name) {
    const playerObject = {
        'id': playerId,
        'name': name,
        'card': 0
    };
    allPlayers.push(playerObject);
    AddOrGetTableCard(playerObject);
    RearangeTableCards();
}

function calculateStatics() {
    let totalPlayersPlaying = 0;
    let totalNumberCards = 0;
    let minCard = 999;
    let maxCard = 0;
    let coffes = 0;
    let question = 0;
    let noVote = 0;
    let sumCardsValue = 0;
    let aggreementPercent = 0;
    let votedCards = {};

    allPlayers.forEach((val, index, array) => {
        const card = val.card;
        if (card > 0) {
            totalPlayersPlaying++;

            if (!votedCards[`${card}`]) {
                votedCards[`${card}`] = 0;
            }
            votedCards[`${card}`]++;

            if (card < 1000) {
                totalNumberCards++;
                sumCardsValue += card;

                if (card < minCard) {
                    minCard = card;
                }
                if (card > maxCard) {
                    maxCard = card;
                }
            } else if (card === 1000) {
                coffes++;
            } else if (card === 1001) {
                question++;
            }
        } else if (card === 0) {
            noVote++;
        }
    });

    if (totalPlayersPlaying > 0) {
        let maxVotedProp = 0;
        let maxVotedCard = 0;
        for (var prop in votedCards) {
            if (votedCards[prop] > maxVotedCard) {
                maxVotedCard = votedCards[prop];
                maxVotedProp = prop;
            }
        }
        aggreementPercent = Math.round((votedCards[maxVotedProp] / totalPlayersPlaying) * 100);
    }
    console.log(votedCards);
    let average = totalNumberCards === 0 ? 0 : Math.round((sumCardsValue / totalNumberCards) * 10) / 10;

    document.getElementById('maxCard').textContent = `${maxCard === 0 ? '-' : maxCard}`;
    document.getElementById('minCard').textContent = `${minCard === 999 ? '-' : minCard}`;
    document.getElementById('averageCard').textContent = `${average === 0 ? '-' : average}`;
    document.getElementById('coffeCards').innerHTML = `&#9749 ${coffes}`;
    document.getElementById('questionCards').innerHTML = `&#10068 ${question}`;
    document.getElementById('noVoteCards').innerHTML = `&#10060 ${noVote}`;
    document.getElementById('aggreementCards').innerHTML = `&#128077 ${aggreementPercent}%`;
}

function AddOrGetTableCard(playerObject) {
    let tableCardId = `tableCard${playerObject.id}`;
    let tableCard = document.getElementById(tableCardId);

    if (!tableCard) {
        tableCard = document.createElement('div');
        tableCard.id = tableCardId;
        tableCard.classList.add('tableCard');
        tableCard.style.opacity = 0;
        setTimeout(() => tableCard.style.opacity = 1, 50);

        const cardLabel = document.createElement('div');
        cardLabel.className = 'tableCardLabel';
        cardLabel.textContent = playerObject.name;
        tableCard.appendChild(cardLabel);

        const cardBg = document.createElement('div');
        cardBg.className = 'tableCardBg';
        tableCard.appendChild(cardBg);

        const cardNumber = document.createElement('div');
        cardNumber.id = `tableCardNumber${playerObject.id}`;
        cardNumber.className = 'tableCardNumber';
        if (playerObject.card === -1) {
            tableCard.classList.add('tableCardObserver');
            cardNumber.innerHTML = '&#128374;';
        }
        cardBg.appendChild(cardNumber);


        tableCardsContainer.appendChild(tableCard);
    }

    return tableCard;
}

function setTableCardNumber(playerId, number) {
    let cardNumber = document.getElementById(`tableCardNumber${playerId}`);

    if (number === 0) {
        cardNumber.innerHTML = areCardsRevealed ? '&#10060;' : '&#8987;';
    } else if (number > 0) {
        if (areCardsRevealed) {
            setTimeout(() => {
                cardNumber.innerHTML = number === 1000 ? '&#9749' : (number === 1001 ? '&#10068' : number);
            }, 200);
        } else {
            cardNumber.innerHTML = '';
        }
    }

}

function RemoveTableCard(playerId) {
    const tableCard = document.getElementById(`tableCard${playerId}`);
    if (tableCard) {
        tableCard.style.opacity = 0;
        tableCard.id = `${tableCard.id}toDelete${Math.random()}`;
        setTimeout(() => tableCard.remove(), 500);
    }
}

function RearangeTableCards() {
    const radius = 180;

    allPlayers.forEach((val, index, array) => {

        const tableCard = AddOrGetTableCard(val);
        setTableCardNumber(val.id, val.card);

        // Set Postiion
        const angle = (index * (360 / allPlayers.length) - 90) * (Math.PI / 180);
        const x = Math.cos(angle) * radius + 50;
        const y = Math.sin(angle) * radius + 50;
        tableCard.style.transform = `translate(-50%, -50%) translate(${x - 50}px, ${y + 60}px)`

        // Set Style
        const wasShown = tableCard.classList.contains('tableCardShown');
        tableCard.classList.remove('tableCardShown');
        tableCard.classList.remove('tableCardShowning');
        tableCard.classList.remove('tableCardHidden');
        tableCard.classList.remove('tableCardNotSelected');

        if (val.card === 0) {
            tableCard.classList.add('tableCardNotSelected');
        } else if (val.card > 0) {
            if (areCardsRevealed) {
                if (!wasShown) {
                    tableCard.classList.add('tableCardHidden');
                    tableCard.classList.add('tableCardShowning');
                    setTimeout(() => {
                        tableCard.classList.add('tableCardShown');
                        tableCard.classList.remove('tableCardHidden');
                    }, 200);
                } else {
                    tableCard.classList.add('tableCardShown');
                }
            } else {
                tableCard.classList.add('tableCardHidden');
            }
        }

    });
}

function tableCardSelected(playerId, selectedCard) {
    const playerObject = allPlayers.find(player => player.id === playerId);
    if (playerObject) {
        playerObject.card = selectedCard;
    }
    RearangeTableCards();
}

function playerCardSelected(selectedCard) {
    for (const val of playerCardsContainer.children) {
        val.classList.remove('playerCardLoading');

        if (val.getAttribute('number') == selectedCard) {
            val.classList.add('playerCardSelected');
        } else {
            val.classList.remove('playerCardSelected');
        }
    }
    tableCardSelected(myId, selectedCard);
}

function addPlayerCard(text, number, index = 0, totalCards = 1) {
    const playerCard = document.createElement('div');
    playerCard.id = `playerCard${number}`;
    playerCard.classList.add('playerCard');
    playerCard.classList.add('playerCardHidden');
    playerCard.setAttribute('number', number);
    playerCard.onclick = function () {
        if (!isRevealCardsSent) {
            playerCard.classList.add('playerCardLoading');
            sendCardSelected(number);
        }
    };

    playerCard.style.marginRight = '-40px';

    const cardBg = document.createElement('div');
    cardBg.className = 'playerCardBg';
    playerCard.appendChild(cardBg);

    const cardNumber = document.createElement('div');
    cardNumber.id = `playerCardNumber${number}`;
    cardNumber.className = 'playerCardNumber';
    cardBg.appendChild(cardNumber);

    playerCardsContainer.appendChild(playerCard);
    setTimeout(() => {
        playerCard.style.marginRight = '5px';
    }, 500 + 50 * index);

    setTimeout(() => {
        playerCard.classList.add('playerCardShowing');
        setTimeout(() => {
            cardNumber.innerHTML = text;
            playerCard.classList.add('playerCardShown');
        }, 200);
    }, 700 + 50 * index + 50 * totalCards);
}

function fibonacci(n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

function generateAndShowPlayerCards() {
    if (!isExpectator) {
        console.log('generateAndShowPlayerCards');
        playerCardsContainer.innerHTML = '';

        const cardsAmount = 12;
        const minWidth = Math.min(window.innerWidth - 40, cardsAmount * (35 + 12));
        const cardsRows = Math.ceil((cardsAmount * (35 + 12)) / minWidth) * 70;
        console.log(`minwidth ${minWidth}, cardsRows ${cardsRows}`);

        playerCardsContainer.style.minWidth = `${minWidth}px`;
        playerCardsContainer.style.minHeight = `${Math.max(70, cardsRows)}px`;;

        for (let i = 2; i <= cardsAmount - 1; i++) {
            const fib = fibonacci(i);
            addPlayerCard(fib, fib, i - 1, cardsAmount);
        }
        addPlayerCard('&#9749;', 1000, 10, cardsAmount);
        addPlayerCard('&#10068;', 1001, 11, cardsAmount);
    }
}

window.onload = function () {
    console.log('Windowd Loaded');
    loginContainer = document.getElementById('loginContainer');
    logoImage = document.getElementById('logoImage');
    gameContainer = document.getElementById('gameContainer');
    tableCardsContainer = document.getElementById('tableCardsContainer');
    playerCards = document.getElementById('playerCards');
    playerCardsContainer = document.getElementById('playerCardsContainer');
    staticsContainer = document.getElementById('staticsContainer');
    btnNewGame = document.getElementById('btnNewGame');
    btnRevealCards = document.getElementById('btnRevealCards');
    loadingLayer = document.getElementById('loadingLayer');
    errorLayer = document.getElementById('errorLayer');
    errorText = document.getElementById('errorText');

    RearangeTableCards();
    webSocketConect();
};