<template>
  <div id="app">
    <div class="block">
      <h1 class="heading">File Upload</h1>
      <FileRow/>
    </div>

    <div class="block">
      <h1 class="heading">File Explorer</h1>
      <FileExplorer/> 
    </div>

    <div class="block">
      <h1 class="heading">MCU Status</h1>
      <div id="STATUS">
        <h1>Websocket:</h1>
        <h1 id="STOK" v-if="WSstatus">OK</h1>
        <h1 id="STER" v-else>ERROR</h1>
      </div>
      <div id="STATUS">
        <h1>Metrics:</h1>
        <h1 id="STOK" v-if="WSstatus">OK</h1>
        <h1 id="STER" v-else>ERROR</h1>
      </div>
    </div>
  </div>
</template>


<script>
import FileRow from "@/components/FileRow"
import FileExplorer from "@/components/FileExplorer"


export default {
  name: 'App',
  components:{FileRow, FileExplorer},
  data: function() {
    return {
      WSstatus: false,
      MetricsStatus: false,
      connection: null
    }
  },
  methods: {
    async ProcessMetricsJSON(resp){
      resp.then(metrics=>{
        try{ metrics = metrics.json(); }
        catch{ this.MetricsStatus = false; console.log("eeee")}
        console.log(metrics);
        this.MetricsStatus = true;
      })
    }
  },
  created: function() {

    ///////////////////////////////////////
    //    Websockets
    //
    
    console.log("Starting connection to WebSocket Server...")
    // this.connection = new WebSocket(`ws://127.0.0.1:2020/`) // local test
    // this.connection = new WebSocket(`ws://${window.location.hostname}`) // MCU
    this.connection = new WebSocket(`ws://192.168.0.196`) // MCU

    this.connection.onerror = function(event){
      this.WSstatus = false;
    }

    this.connection.onmessage = function(event) {
      console.log(event);
      if(event.data === "UPDATE") {
        console.log("Updating state...")
      }
    }

    this.connection.onopen = function(event) {
      console.log(event)
      console.log("Successfully connected to the websocket server...")
      this.WSstatus = true;
    }

    this.connection.onclose = function(event) {
      console.log(event)
      console.log("Successfully closed the websocket connection...")
    } 

    ///////////////////////////////////////
    //    Metrics Fetch
    //

    // let getMetrics = () => {fetch("http:127.0.0.1/metrics").then(resp => this.ProcessMetricsJSON(resp)).catch(e => this.MetricsStatus = false);; } // local test
    let getMetrics = () => {fetch(`http:192.168.0.196/metrics`).then(resp => this.ProcessMetricsJSON(resp)).catch(e => this.MetricsStatus = false); } // MCU ${window.location.hostname}
    console.log(window.location.hostname)
    setInterval(getMetrics, 1000);


  }
}
</script>

<style lang="scss">
*{
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

#app {
  display: flex;
  flex-direction: column;
  align-items: center;
  
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;

  .block{
    padding-top: 5%;
    
    .heading{
      text-align: center;
      text-decoration-line: underline;
    }
  }

  #STATUS{
    display: flex;
    flex-direction: row;
    #STOK{
      color: green;
    }
    #STER{
      color: red;
    }
  }
}
</style>