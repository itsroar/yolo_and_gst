#include "utils.h"

#include <gst/gst.h>

#include <iostream>

static void on_demux_pad_added(GstElement *demux, GstPad *new_pad, GstElement *pipeline)
{
    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(demux));

    GstCaps *new_pad_caps = gst_pad_get_current_caps(new_pad);
    GstStructure *new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    const gchar *new_pad_type = gst_structure_get_name(new_pad_struct);

    GstElement *connecting_elem = nullptr;
    if (g_str_has_prefix(new_pad_type, "video"))
        connecting_elem = gst_bin_get_by_name(GST_BIN(pipeline), "queue_video");
    else if (g_str_has_prefix(new_pad_type, "audio"))
        connecting_elem = gst_bin_get_by_name(GST_BIN(pipeline), "queue_audio");

    if (connecting_elem == nullptr)
    {
        gst_caps_unref(new_pad_caps);
        return;
    }

    GstPad *sink_pad = gst_element_get_static_pad(connecting_elem, "sink");
    if (gst_pad_is_linked(sink_pad))
    {
        g_print("%s is already linked. Ignoring.\n", GST_PAD_NAME(sink_pad));
        goto end;
    }
    else
    {
        GstPadLinkReturn ret = gst_pad_link(new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED(ret))
            g_print("Type is '%s' but link failed.\n", new_pad_type);
        else
            g_print("Link succeeded (type '%s').\n", new_pad_type);
    }

end:
    gst_caps_unref(new_pad_caps);
    gst_object_unref(sink_pad);
}

static void on_videodecoder_ready(GstElement *decodebin, GstPad *new_pad, GstElement *pipeline)
{
    GstElement *videorate = gst_bin_get_by_name(GST_BIN(pipeline), "videorate");
    GstPad *sink_pad = gst_element_get_static_pad(videorate, "sink");
    if (gst_pad_is_linked(sink_pad))
    {
        gst_object_unref(sink_pad);
        g_print ("We are already linked. Ignoring.\n");
        return;
    }

    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(decodebin));

    GstCaps *new_pad_caps = gst_pad_get_current_caps(new_pad);
    GstStructure *new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    const gchar *new_pad_type = gst_structure_get_name(new_pad_struct);

    if (g_str_has_prefix(new_pad_type, "video"))
    {
        GstPadLinkReturn ret = gst_pad_link(new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED(ret))
            g_print("Type is '%s' but link failed.\n", new_pad_type);
        else
            g_print("Link succeeded (type '%s').\n", new_pad_type);
    }

    gst_caps_unref(new_pad_caps);
    gst_object_unref(sink_pad);
}

static void on_audiodecoder_ready(GstElement *audiodecoder, GstPad *new_pad, GstElement *pipeline)
{
    GstElement *audioconvert = gst_bin_get_by_name(GST_BIN(pipeline), "audioconvert");
    GstPad *sink_pad = gst_element_get_static_pad(audioconvert, "sink");
    if (gst_pad_is_linked(sink_pad))
    {
        gst_object_unref(sink_pad);
        g_print ("We are already linked. Ignoring.\n");
        return;
    }

    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(audiodecoder));

    GstCaps *new_pad_caps = gst_pad_get_current_caps(new_pad);
    GstStructure *new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    const gchar *new_pad_type = gst_structure_get_name(new_pad_struct);

    if (g_str_has_prefix(new_pad_type, "audio"))
    {
        GstPadLinkReturn ret = gst_pad_link(new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED(ret))
            g_print("Type is '%s' but link failed.\n", new_pad_type);
        else
            g_print("Link succeeded (type '%s').\n", new_pad_type);
    }

    gst_caps_unref(new_pad_caps);
    gst_object_unref(sink_pad);
}

static GstPadProbeReturn on_pad_probe(GstPad *pad, GstPadProbeInfo *info, gpointer /*user_data*/)
{
    GstBuffer *buffer = gst_pad_probe_info_get_buffer(info);

    GstMapInfo map_info;
    gst_buffer_map(buffer, &map_info, GST_MAP_READ);

    GstCaps *caps = gst_pad_get_current_caps(pad);
    GstStructure *structure = gst_caps_get_structure(caps, 0);
    int w, h;
    gst_structure_get_int(structure, "width", &w);
    gst_structure_get_int(structure, "height", &h);
    gst_caps_unref(caps);

    std::vector<ObjectDetection> objects;
    detectYoloV3(map_info.data, w, h, objects);
    drawObjectDetections(map_info.data, w, h, objects);

    gst_buffer_unmap(buffer, &map_info);

    return GST_PAD_PROBE_OK;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage:\n\tyolo_and_gst <videofile_path>\n\tyolo_and_gst <videofile_path> <fps>\n\n");
        printf("Example:\n\tyolo_and_gst \"/home/user/video.mp4\"\n\tyolo_and_gst \"/home/user/video.mp4\" 10\n\n");
        printf("Default fps is 10\n\n");
        exit(-1);
    }

    char *filepath = argv[1];
    int fps = (argc < 3) ? 10 : atoi(argv[2]);

    gst_init(&argc, &argv);
    initUtils();

    GstElement *pipeline = gst_pipeline_new("videoshow");

    GstElement *filesrc = gst_element_factory_make("filesrc", "videofile");
    g_object_set(filesrc, "location", filepath, NULL);

    GstElement *demux = gst_element_factory_make("qtdemux", "demux");
    g_signal_connect(demux, "pad-added", G_CALLBACK(on_demux_pad_added), pipeline);

    GstElement *queue_video = gst_element_factory_make("queue", "queue_video");
    GstElement *decodebin_video = gst_element_factory_make("decodebin", "decodebin_video");
    // link decodebin with other elements after some time, when decodebin read file metadata
    g_signal_connect(decodebin_video, "pad-added", G_CALLBACK(on_videodecoder_ready), pipeline);

    GstElement *videorate = gst_element_factory_make("videorate", "videorate");
    g_object_set(videorate, "max-rate", fps, NULL);

    GstElement *videoconvert1 = gst_element_factory_make("videoconvert", "videoconvert1");
    // setting our callback for edit video
    GstPad *vc_output = gst_element_get_static_pad(videoconvert1, "src");
    gst_pad_add_probe(vc_output, GST_PAD_PROBE_TYPE_BUFFER, on_pad_probe, NULL, NULL);
    gst_object_unref(vc_output);

    GstElement *capsfilter = gst_element_factory_make("capsfilter", "rgb_filter");
    GstCaps *caps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
    gst_caps_unref(caps);

    GstElement *videoconvert2 = gst_element_factory_make("videoconvert", "videoconvert2");
    GstElement *videosink = gst_element_factory_make("autovideosink", "videosink");

    GstElement *queue_audio = gst_element_factory_make("queue", "queue_audio");
    GstElement *decodebin_audio = gst_element_factory_make("decodebin", "decodebin_audio");
    g_signal_connect(decodebin_audio, "pad-added", G_CALLBACK(on_audiodecoder_ready), pipeline);

    GstElement *audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
    GstElement *audiosink = gst_element_factory_make("autoaudiosink", "audiosink");

    if (!pipeline || !filesrc || !demux || !queue_video || !decodebin_video ||
        !videorate || !videoconvert1 || !capsfilter || !videoconvert2 || !videosink ||
        !queue_audio || !decodebin_audio || !audioconvert || !audiosink)
    {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    gst_bin_add_many(GST_BIN(pipeline), filesrc, demux,
                     queue_video, decodebin_video, videorate, videoconvert1, capsfilter, videoconvert2, videosink,
                     queue_audio, decodebin_audio, audioconvert, audiosink, NULL);

    if (gst_element_link(filesrc, demux) != TRUE ||
        gst_element_link(queue_video, decodebin_video) != TRUE ||
        gst_element_link_many(videorate, videoconvert1, capsfilter, videoconvert2, videosink, NULL) != TRUE ||
        gst_element_link(queue_audio, decodebin_audio) != TRUE ||
        gst_element_link(audioconvert, audiosink) != TRUE)
    {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n",
                        GST_OBJECT_NAME (msg->src), err->message);
            g_printerr("Debugging information: %s\n",
                        debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;
        default:
            /* We should not reach here because we only asked for ERRORs and EOS */
            g_printerr("Unexpected message received.\n");
            break;
        }
        gst_message_unref(msg);
    }

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}
