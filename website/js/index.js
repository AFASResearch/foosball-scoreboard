document.addEventListener('DOMContentLoaded', function () {
  var h = maquette.h;
  var domNode = document.body;
  var projector = maquette.createProjector();

  var scoreEntries = [
    { name: 'RHG', score: '3450'},
    { name: 'WKL', score: '3320'},
    { name: 'WPO', score: '3100'},
    { name: 'JVO', score: '2830'},
    { name: 'QMO', score: '2600'},
    { name: 'AME', score: '2410'},
    { name: 'GSC', score: '2150'},
    { name: 'MOV', score: '1000'},
    { name: 'AHO', score: '800'},
    { name: 'BVL', score: '1'}
  ];

  var getRandomColorCode = () => {
    let code = Math.floor(Math.random()*16777215).toString(16);
    
    return '#'+code;
  };

  var getDistinctRandomIntForArray = (array, range) => {
    var n = Math.floor((Math.random() * range));
    if(array.indexOf(n) == -1){        
      return n; 
    } else {
      return getDistinctRandomIntForArray(array, range); 
    }
  }

  var generateArrayOfRandomInts = (count, range) => {
    var array = []; 
    for (i=0; i<count; ++i){
      array[i] = getDistinctRandomIntForArray(array, range);
    };
    return array; 
  }

  var drawCanvas = () => {
    var canvas = document.getElementById("canvas");
    if (canvas && canvas.getContext) {
      var randomXNumbers = generateArrayOfRandomInts(100, window.innerWidth);
      var randomYNumbers = generateArrayOfRandomInts(100, window.innerHeight);
      canvas.width = window.innerWidth;
      canvas.height = window.innerHeight;
      var context = canvas.getContext("2d");

      for(var i = 0; i < 100; i++) {
        context.fillStyle = "rgb(255,255,255)";
        context.fillRect (randomXNumbers[i], randomYNumbers[i], 3, 3);
      }
    } else {
      setTimeout(drawCanvas, 100);
    }
  };

  drawCanvas();

  var page =  {
    renderMaquette: () => {
      return h('div.container', [
        h('div.background', [
          h('canvas#canvas')
        ]),
        h('div.mainContainer', [
          h('div.title', 'GAME SCORES'),
          h('div.scores', [
            h('div.gameTitle', 'DONKEY KONG'),
            h('div.scoreEntry.header', [
              h('div.rank', 'RANK'),
              h('div.playerName', 'NAME'),
              h('div.playerScore', 'SCORE')
            ]),
            scoreEntries.map((score, index) => {
              return h('div.scoreEntry', {
                styles: {
                  color: getRandomColorCode()
                }
              }, [
                h('div.rank', 'RANK ' + (index + 1) ),
                h('div.playerName', score.name),
                h('div.playerScore', score.score)
              ])
            })
          ])
        ])
      ]);
    }
  };

  projector.append(domNode, page.renderMaquette);

});