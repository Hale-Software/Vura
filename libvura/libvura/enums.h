#pragma once


enum class PlaybackState
{
    Stopped,
    Playing,
    Paused
};

enum class MediaStatus
{
    NoMedia,
    Loading,
    Loaded,
    Buffering,
    Buffered,
    EndOfMedia,
    Invalid
};

enum class ErrorKind
{
    None,
    NotFound,
    AccessDenied,
    UnsupportedFormat,
    MissingCodec,
    NetworkFailure,
    DecodeFailure,
    Internal
};

enum class OutputKind
{
    VideoSink,
    GLContext,
    MemoryBuffer
};
