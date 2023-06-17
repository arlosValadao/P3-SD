// Get the HTML canvas by its id 
plots = document.getElementById("plots");
// Example datasets for X and Y-axes 
var months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul"]; //Stays on the X-axis 
var traffic = [65, 59, 80, 81, 56, 55, 60] //Stays on the Y-axis 
// Create an instance of Chart object:
new Chart(plots, {
    type: 'line', //Declare the chart type 
    data: {
        labels: months, //X-axis data 
        datasets: [{
            data: traffic, //Y-axis data 
            backgroundColor: '#5e440f',
            borderColor: 'white',
            fill: false, //Fills the curve under the line with the babckground color. It's true by default 
        }]
    },
});