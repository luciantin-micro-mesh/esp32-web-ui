<template>
  <div class="FileRow">
    <h1>{{ msg }}</h1>
    <div class = "upload">
      <form method = "POST" action = "/doUpload" enctype="multipart/form-data">
          <input type="file" name="data"/>
          <input type="submit" name="upload" value="Upload" title = "Upload Files">
      </form>	
    </div>
  </div>
</template>

<script>
export default {
  name: 'FileRow',
  props: {
    msg:String,
  }
}
</script>

<style scoped lang="scss">

.FileRow {
  display: flexbox;
  flex-direction: row;
}

h3 {
  margin: 40px 0 0;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
</style>
