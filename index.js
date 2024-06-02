const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector("#authentication-bar");
var temperatureData = [];
var humidityData = [];

// Elements for sensor readings
const tempElement = document.getElementById("temp");
const qualElement = document.getElementById("quality");
const altElement = document.getElementById("alt");
const humElement = document.getElementById("hum");
const presElement = document.getElementById("pres");
const reportElement = document.getElementById("report");
const voltajeElement = document.getElementById("volt");
const ledElement = document.getElementById("led"); 

var dbPathLed;

// MANAGE LOGIN/LOGOUT UI
const setupUI = (user) => {
  if (user) {
    //toggle UI elements
    loginElement.style.display = 'none';
    contentElement.style.display = 'block';
    authBarElement.style.display ='block';
    userDetailsElement.style.display ='block';
    userDetailsElement.innerHTML = user.email;

    // get user UID to get data from database
    var uid = user.uid;
    console.log(uid);

    // Database paths (with user UID)
    var dbPathTemp = 'UsersData/' + uid.toString() + '/temperature';
    var dbPathQual = 'UsersData/' + uid.toString() + '/quality';
    var dbPathAlt = 'UsersData/' + uid.toString() + '/altitude';
    var dbPathHum = 'UsersData/' + uid.toString() + '/humidity';
    var dbPathPres = 'UsersData/' + uid.toString() + '/pressure';
    var dbPathReport = 'UsersData/' + uid.toString() + '/UltimoReporte';
    var dbPathVoltaje = 'UsersData/' + uid.toString() + '/voltaje';
    dbPathLed = 'UsersData/' + uid.toString() + '/led';

    // Database references
    var dbRefTemp = firebase.database().ref().child(dbPathTemp);
    var dbRefQual = firebase.database().ref().child(dbPathQual);
    var dbRefAlt = firebase.database().ref().child(dbPathAlt);
    var dbRefHum = firebase.database().ref().child(dbPathHum);
    var dbRefPres = firebase.database().ref().child(dbPathPres);
    var dbRefReport = firebase.database().ref().child(dbPathReport);
    var dbRefVoltaje = firebase.database().ref().child(dbPathVoltaje);

    // Crear el manómetro
    var gaugeTemp = new JustGage({
      id: "gauge-temp",
      value: 0,
      min: 0,
      max: 100,
      title: "Temperatura",
      label: "°C",
      titleFontSize: 30, // Tamaño del título
      labelFontSize: 20, // Tamaño de la etiqueta
      titleFontColor: "#000000", // Color del título
      labelFontColor: "#000000" // Color de la etiqueta
    });
    var gaugePress = new JustGage({
      id: "gauge-press",
      value: 0,
      min: 0,
      max: 1500,
      title: "Presión",
      label: "hPa",
      titleFontSize: 30, // Tamaño del título
      labelFontSize: 20, // Tamaño de la etiqueta
      titleFontColor: "#000000", // Color del título
      labelFontColor: "#000000" // Color de la etiqueta
    });
    var gaugeHum = new JustGage({
      id: "gauge-hum",
      value: 0,
      min: 0,
      max: 100,
      title: "Humedad",
      label: "%",
      titleFontSize: 30, // Tamaño del título
      labelFontSize: 20, // Tamaño de la etiqueta
      titleFontColor: "#000000", // Color del título
      labelFontColor: "#000000" // Color de la etiqueta
    });   

    // Update page with new readings
    // Se agrega el primer punto al array (para evitar que la gráfica comience vacía)
    temperatureData.push([new Date().getTime(), 0]);
    humidityData.push([new Date().getTime(), 0]);

    // Update page with new readings GRAFICA TEMPERATURA
    dbRefTemp.on('value', snap => {
      var tempValue = snap.val().toFixed(0);
      tempElement.innerText = tempValue + " °C"; // Añade °C
      var x = (new Date()).getTime(),
          y = parseFloat(tempValue);

      // Actualiza el manómetro
      gaugeTemp.refresh(y);

      // Se agrega un nuevo punto al array
      temperatureData.push([x, y]);

      // Se actualiza la gráfica con el nuevo array de datos
      chartT.series[0].setData(temperatureData);

      // Se reinicia el temporizador cada vez que se actualiza la gráfica
      clearInterval(updateInterval);
      updateInterval = setInterval(updateChart, 60000); // 1 minuto
    });

    // GRAGICA DE HUMEDAD
    dbRefHum.on('value', snap => {
      humElement.innerText = snap.val().toFixed(2); // Actualiza el valor mostrado en el HTML
      var x = (new Date()).getTime(),
          y = parseFloat(snap.val().toFixed(2));

      // Agrega un nuevo punto al array de datos de humedad
      humidityData.push([x, y]);

      // Actualiza la gráfica de humedad con el nuevo array de datos
      chartHumidity.series[0].setData(humidityData);

      // Reinicia el temporizador cada vez que se actualiza la gráfica
      clearInterval(updateIntervalHumidity);
      updateIntervalHumidity = setInterval(updateChartHumidity, 60000); // 1 minuto
    });

    // Función para actualizar la gráfica cada minuto
    function updateChart() {
      var x = (new Date()).getTime(),
          y = temperatureData[temperatureData.length - 1][1]; // Se toma el último valor de la temperatura

      // Se agrega un nuevo punto al array
      temperatureData.push([x, y]);

      // Se actualiza la gráfica con el nuevo array de datos
      chartT.series[0].setData(temperatureData);
    }

    // Función para actualizar la gráfica de humedad cada minuto
    function updateChartHumidity() {
      var x = (new Date()).getTime(),
          y = humidityData[humidityData.length - 1][1]; // Toma el último valor de la humedad

      // Agrega un nuevo punto al array de datos de humedad
      humidityData.push([x, y]);

      // Actualiza la gráfica de humedad con el nuevo array de datos
      chartHumidity.series[0].setData(humidityData);
    }

    // Se inicia el temporizador al cargar la página
    var updateInterval = setInterval(updateChart, 60000); // 1 minuto
    var updateIntervalHumidity = setInterval(updateChartHumidity, 30000); // 1 minuto

    dbRefHum.on('value', snap => {
      var humValue = snap.val().toFixed(2);
      humElement.innerHTML = humValue + " %";
      var y = parseFloat(humValue);

      // Actualiza el manómetro de presión
      gaugeHum.refresh(y);
    });

    // Actualización del manómetro de presión
    dbRefPres.on('value', snap => {
      var presValue = snap.val().toFixed(2);
      presElement.innerHTML = presValue + " hPa";
      var y = parseFloat(presValue);

      // Actualiza el manómetro de presión
      gaugePress.refresh(y);
    });

    dbRefQual.on('value', snap => {
      qualElement.innerText = snap.val().toFixed(2);
    });

    dbRefAlt.on('value', snap => {
      altElement.innerText = snap.val().toFixed(2);
    });

    dbRefReport.on('value', snap => {
      reportElement.innerText = snap.val();
    });

    dbRefVoltaje.on('value', snap => {
      voltajeElement.innerText = snap.val().toFixed(2);
    });

  // if user is logged out
  } else {
    // toggle UI elements
    loginElement.style.display = 'block';
    authBarElement.style.display ='none';
    userDetailsElement.style.display ='none';
    contentElement.style.display = 'none';
  }
}

function toggleLed() {
  console.log("Toggle");
  if (ledElement.checked) {
    console.log("led ON");
    firebase.database().ref(dbPathLed).set("ON");
  } else {
    console.log("led OFF");
    firebase.database().ref(dbPathLed).set("OFF");
  }
}
