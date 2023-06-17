function getdevice(){
    var requests = $.get('/');

    var tm = requests.done(function (result){
        var today = new Date();
        var time = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
        var dados = document.getElementById('dados').value;
        var dados = JSON.parse(dados)

        var digitais = dados['d']
        var analogico = dados['a']
        for(var i = 1; i < 9; i++){
            var d = document.getElementById(String(i));
            //temperature chart object created 
            var d_card = new Chart(d, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Sensor digital',
                        data: [],
                        fill:true,
                        backgroundColor: 'rgba(244, 67, 54, 0.1)',
                        borderColor:'rgba(244, 67, 54, 1)',
                        borderWidth: 3
                    }]
                },
                options: {
                    scales: {
                        yAxes: [{
                            ticks: {
                                beginAtZero: true
                            }
                        }]
                    }
                }
            });
            
            addData(d_card, time, digitais[String(i)]);
            if (couter >= 10 ){
                removeData(d_card);
            }
            couter++;
        }
   

        setTimeout(getdevice, 2000);
        
    
    });
    
}




function addData(chart, label, data) {
    chart.data.labels.push(label);
    chart.data.datasets.forEach((dataset) => {
        dataset.data.push(data);
    });
    chart.update();
}

function removeData(chart) {
    chart.data.labels.shift();
    chart.data.datasets.forEach((dataset) => {
        dataset.data.shift();
    });
    chart.update();
}

var couter = 0; 

getdevice();
