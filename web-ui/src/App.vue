<template>
  <div id="app">
    <div class="block">
      <h1 class="heading">File Upload</h1>
      <FileRow/>
    </div>

    <div class="block">
      <h1 class="heading">Files</h1>
      <div v-for="file in files" v-bind:key="file">
        <h2>{{file}}</h2>
      </div>
    </div>

    <div class="block">
      <h1 class="heading">WASM ports</h1>
      <div v-for="(port, mod) in ports" v-bind:key="port">
        <h2>{{mod}} port --> {{port}}</h2>
        <a :href="getHostname(port)" > {{getHostname(port)}} </a>
      </div>
    </div>

    <div class="block">
      <h1 class="heading">MCU Status</h1>
      <div id="STATUS">
        <h1>Websocket:</h1>
        <h1 id="STOK" v-if="WSstatus==true">OK</h1>
        <h1 id="STER" v-else>ERROR</h1>
      </div>
      <div id="STATUS">
        <h1>Metrics:</h1>
        <h1 id="STOK" v-if="MetricsStatus==true">OK</h1>
        <h1 id="STER" v-else>ERROR</h1>
      </div>
      <h1>Used Flash Bytes : {{UsedBytes}}/400000</h1>
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
      connection: null,
      files: [],
      ports:{},
      UsedBytes:-1,
    }
  },
  methods: {
    async ProcessMetricsJSON(resp){
      resp.json().then(metrics=>{
        // console.log(metrics);
        this.MetricsStatus = true;
        this.files = metrics["files"];
        this.ports = metrics["ports"];
        this.UsedBytes = metrics["Used bytes"];
      })
    },

    getHostname(port){
      return window.location.protocol + "//" + window.location.hostname + ":" + String(port);
    },

    WS_onmessage(event){
      console.log(event);
      if(event.data === "UPDATE") {
        console.log("Updating state...")
      }
    },

    WS_onclose(event){
      this.WSstatus = false;
      console.log(event)
      console.log("Successfully closed the websocket connection...")
      setTimeout(this.startWS,10000);
    },

    WS_onopen(event){
      console.log(event)
      console.log("Successfully connected to the websocket server...")
      this.WSstatus = true;
    },

    WS_onerror(event){
      console.log(event)
      this.WSstatus = false;
      this.connection.close();
    },
    getMetrics(){
      const controller = new AbortController();
      const timeoutId = setTimeout(() => controller.abort(), 2000)
      // http://192.168.0.196/metrics
      fetch(`/metrics`, { signal: controller.signal })
        .then(resp => this.ProcessMetricsJSON(resp))
        .catch(e => {this.MetricsStatus = false; console.log(e)});
    }, // MCU ${window.location.hostname}
    startWS(){
      ///////////////////////////////////////
      //    Websockets
      //
      
      console.log("Starting connection to WebSocket Server...")
      // this.connection = new WebSocket(`ws://127.0.0.1:2020/`) // local test
      this.connection = new WebSocket(`ws://${window.location.hostname}/ws`) // MCU
      // this.connection = new WebSocket(`ws://192.168.0.196/ws`) // MCU
      
      // this.connection.onerror = this.WS_onerror;
      this.connection.onerror = (e)=>{console.log("WS ERRRRRRRRRORRRRRRR");console.log(e)};

      this.connection.onmessage = this.WS_onmessage;

      this.connection.onopen = this.WS_onopen;

      this.connection.onclose = this.WS_onclose;

      // setTimeout(this.startWS,10000)
    }
  },
  created: function() {

    this.startWS();

    ///////////////////////////////////////
    //    Metrics Fetch
    //
    // let getMetrics = () => {fetch("http:127.0.0.1/metrics").then(resp => this.ProcessMetricsJSON(resp)).catch(e => this.MetricsStatus = false);; } // local test
    // let getMetrics = () => {fetch(`http://192.168.0.196/metrics`).then(resp => this.ProcessMetricsJSON(resp)).catch(e => this.MetricsStatus = false); } // MCU ${window.location.hostname}
    // let getMetrics = () => {fetch(`/metrics`).then(resp => this.ProcessMetricsJSON(resp)).catch(e => this.MetricsStatus = false); } // relative path to hostname
    console.log(window.location.hostname)
    setInterval(this.getMetrics, 2000);
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