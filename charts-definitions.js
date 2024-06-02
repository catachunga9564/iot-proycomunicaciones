window.addEventListener('load', onload);

function onload(event){
  chartT = createTemperatureChart();
  chartQ = createQualityChart();
  chartH = createHumidityChart();
  chartP = createPressureChart();
  chartA = createAltitudeChart();
  chartR = createReportChart();
  chartV = createVoltajeChart();
}

Highcharts.setOptions({
  time:{
    timezone:'America/Bogota'
  }
});

// Create Temperature Chart
function createTemperatureChart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-temperature',
      type: 'spline' 
      
    },
    series: [
      {
        name: 'BMP280'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
        enabled: true 
        
        }
      }
    },

    tooltip: {
      formatter: function () {
        return '<b>' + Highcharts.dateFormat('%Y-%m-%d %H:%M:%S', this.x) + '</b><br/>' +
          'Temperatura: ' + this.y.toFixed(2) + ' °C';
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { minute: '%H:%M'}
      
    },
    yAxis: {
      title: { 
        text: 'Temperatura °C' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}

// Create Humidity Chart
function createHumidityChart(){
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-humidity',
      type: 'spline'  
    },
    series: [{
      name: 'DHT22'
    }],
    title: { 
      text: undefined
    },    
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      },
      series: { 
        color: '#50b8b4' 
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { minute: '%H:%M' }
    },
    yAxis: {
      title: { 
        text: 'Humidity (%)' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}

// Create Pressure Chart
function createPressureChart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-pressure',
      type: 'spline'  
    },
    series: [{
      name: 'DHT22'
    }],
    title: { 
      text: undefined
    },    
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      },
      series: { 
        color: '#A62639' 
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { minute: '%H:%M' }
    },
    yAxis: {
      title: { 
        text: 'Pressure (hPa)' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}

// Create Quality Chart
function createQualityChart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-pressure',
      type: 'spline'  
    },
    series: [{
      name: 'DHT22'
    }],
    title: { 
      text: undefined
    },    
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      },
      series: { 
        color: '#A64635' 
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { minute: '%H:%M' }
    },
    yAxis: {
      title: { 
        text: 'Quality (ICA)' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}
// Create Altitude Chart
function createAltitudeChart() {
  var chart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-altitude',
      type: 'spline' 
      
    },
    series: [
      {
        name: 'BMP280'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
        enabled: true 
        
        }
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { minute: '%H:%M'}
      
    },
    yAxis: {
      title: { 
        text: 'Altitud m.s.n.m' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return chart;
}

// Crea la gráfica de humedad/////////////////////////////////////////////
const chartHumidity = Highcharts.chart('chartHumidity', {
  chart: {
    type: 'line'
  },
  series: [
    {
      name: 'BMP280'
    }
  ],
  credits: { 
    enabled: false 
  },
  title: {
    text: ''
  },
  xAxis: {
    type: 'datetime',
    tickPixelInterval: 150
  },
  yAxis: {
    title: {
      text: 'Humedad (%)'
    },

    plotLines: [{
      value: 0,
      width: 1,
      color: '#808080'
    }]
  },
  tooltip: {
    formatter: function () {
      return '<b>' + Highcharts.dateFormat('%Y-%m-%d %H:%M:%S', this.x) + '</b><br/>' +
        'Humedad: ' + this.y.toFixed(2) + ' %';
    }
  },
  legend: {
    enabled: false
  },
  exporting: {
    enabled: false
  },
  series: [{
    name: 'Humedad',
    data: []
  }]
});